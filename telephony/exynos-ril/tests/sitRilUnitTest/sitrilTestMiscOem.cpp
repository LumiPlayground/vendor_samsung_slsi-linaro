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

#include "sitrilTestMiscOem.h"
#include "testUtil.h"

static UINT8* buildOemetSetRfDesenseModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_set_rf_desense_mode_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_misc_set_rf_desense_mode_rsp *rsp = (sit_misc_set_rf_desense_mode_rsp *)rspData;
    rsp->cause = 1;

    return rspData;
}

static UINT8* buildOemetSetManualBandModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_set_manual_band_mode_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_misc_set_manual_band_mode_rsp *rsp = (sit_misc_set_manual_band_mode_rsp *)rspData;
    rsp->cause = 1;

    return rspData;
}

static UINT8* buildOemCqiInfoRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_oem_get_cqi_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_oem_get_cqi_info_rsp *rsp = (sit_oem_get_cqi_info_rsp *)rspData;
    rsp->type = 1;
    rsp->cqi_info0 = 2;
    rsp->cqi_info1 = 3;
    rsp->ri = 4;

    return rspData;
}

static UINT8* buildGetRadioNodeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_get_radio_node_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_misc_get_radio_node_rsp *rsp = (sit_misc_get_radio_node_rsp *)rspData;
    memset(rsp->value, 'A', MAX_RADIO_NODE_DATA_LEN);
    rsp->value[MAX_RADIO_NODE_DATA_LEN-1] = 0;

    return rspData;
}

static UINT8* buildGetTcsFciInfo(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    const char fci[] = "test\0";
    UINT16 fciLen = sizeof(fci);
    UINT16 len = sizeof(RCM_HEADER) + fciLen;
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_oem_get_tcs_fci_info_rsp *rsp = (sit_oem_get_tcs_fci_info_rsp *)rspData;
    memcpy(rsp->fci, fci, fciLen);

    return rspData;
}

static UINT8* buildGetTraceDumpCfg(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    *datalen = (UINT16)sizeof(sit_oem_get_trace_dump_cfg_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_oem_get_trace_dump_cfg_rsp *rsp = (sit_oem_get_trace_dump_cfg_rsp *)rspData;
    rsp->config = (BYTE)1;

    return rspData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static UINT8* buildModemDataInd(INT16 sitId, UINT32 *datalen) {
    const unsigned int variableDataSize = 5;
    char variableData[variableDataSize] { 1, 2, 3, 4, 5 };
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_oem_nw_info_ind) + variableDataSize;

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_oem_nw_info_ind *ind = (sit_oem_nw_info_ind *)indData;
    ind->command = 1;
    ind->length = variableDataSize;
    memcpy(ind->data, variableData, variableDataSize);

    return indData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// sitrilTestMiscOem class
void SitrilTestMiscOem::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestMiscOem::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// request|response
TEST_P(SitrilTestMiscOem, oemGeRfDefenseMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_RF_DESENSE_MODE;
    UINT16 sitId = SIT_GET_RF_DESENSE_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_rf_desense_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetRfDefenseMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_RF_DESENSE_MODE;
    UINT16 sitId = SIT_SET_RF_DESENSE_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 10;
    char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, reqDataSize, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + reqDataSize;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildOemetSetRfDesenseModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemGetManualBandMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_MANUAL_BAND_MODE;
    UINT16 sitId = SIT_GET_MANUAL_BAND_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_manual_band_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetManualBandMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_MANUAL_BAND_MODE;
    UINT16 sitId = SIT_SET_MANUAL_BAND_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 10;
    char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, reqDataSize, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + reqDataSize;;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildOemetSetManualBandModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemSetPsensorStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_PSENSOR_SET_STATUS;
    UINT16 sitId = SIT_SET_SENSOR_STATUS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_psensor_set_psensor_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetSarState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SAR_SET_STATE;
    UINT16 sitId = SIT_OEM_SET_SAR_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 1;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_sar_set_sar_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetSarState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SAR_GET_STATE;
    UINT16 sitId = SIT_OEM_GET_SAR_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = sizeof(int);
    //int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_sar_get_sar_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemRssiScanReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SCAN_RSSI;
    UINT16 sitId = SIT_SET_RSSI_SCAN;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 25;
    int reqData[reqDataSize] = {1,1, };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_rssi_scan_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetModemInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_MODEM_INFO;
    UINT16 sitId = SIT_OEM_NW_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 2;
    int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_nw_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RCM_E_RADIO_NOT_AVAILABLE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null due to error rsp
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemModemReset) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_MODEM_RESET;
    UINT16 sitId = SIT_SET_RADIO_POWER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    //int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_radio_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetFuncSwitch) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SWITCH_MODEM_FUNCTION;
    UINT16 sitId = SIT_OEM_SET_FUNC_SWITCH_REQ;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 2;
    int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_func_switch_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetPdcpDiscardTimer) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_REQ_SET_PDCP_DISCARD_TIMER;
    UINT16 sitId = SIT_OEM_SET_PDCP_DISCARD_TIMER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_pdcp_discard_timer_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetSelfLog) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SELFLOG;
    UINT16 sitId = SIT_SET_SELFLOG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 2;
    int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqDataSize), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_selflog_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetSelfLogProfile) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SELFLOG_PROFILE;
    //UINT16 sitId = SIT_SET_SELFLOG_PROFILE;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //There would be file read fail. so IPC would not be built and error will come
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestMiscOem, oemGetSelfLogStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_SELFLOG_STATUS;
    UINT16 sitId = SIT_GET_SELFLOG_STATUS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    //int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, setActivateSim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_ACTIVATE_VSIM;
    UINT16 sitId = SIT_OEM_SET_ACTIVATE_VSIM;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_SetActivateVsim reqData = {1, nullptr, nullptr, nullptr, 1, 1};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_activate_visim_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetCqiInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_CQI_INFO;
    UINT16 sitId = SIT_OEM_GET_CQI_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    //int reqData[reqDataSize] = {1, 0};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildOemCqiInfoRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemSetSarSetting) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SAR_SETTING;
    UINT16 sitId = SIT_OEM_SET_SAR_SETTING;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_sar_setting_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetGmoSwitch) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_GMO_SWITCH;
    UINT16 sitId = SIT_OEM_SET_GMO_SWITCH;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_gmo_switch_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetTcsFci) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_TCS_FCI;
    UINT16 sitId = SIT_OEM_SET_TCS_FCI_REQ;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 13;
    BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00,
                                0x02, 0x00, 0x00, 0x00,
                                '1', '2', '3', '4', 0x00};

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_tcs_fci_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetTcsFciInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_TCS_FCI;
    UINT16 sitId = SIT_OEM_GET_TCS_FCI_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //BYTE reqData[reqDataSize] = {0, };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetTcsFciInfo(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemSetCaBwFilter) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_CA_BANDWIDTH_FILTER;
    UINT16 sitId = SIT_OEM_SET_CA_BW_FILTER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_ca_bw_filter_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetModemLogDump) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_MODEM_LOG_DUMP;
    UINT16 sitId = SIT_SET_MODEM_LOG_DUMP;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_modem_log_dump_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetElevatorSensor) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_ELEVATOR_SENSOR;
    UINT16 sitId = SIT_SET_ELEVATOR_SENSOR;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_elevator_sensor_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetForbidLteCell) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FORBID_LTE_CELL;
    UINT16 sitId = SIT_OEM_SET_FORBID_LTE_CELL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 18;
    BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, '4', '2', '5','1', '1', '1'};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_forbid_lte_cell_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetApSystemTime) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_AP_SYSTEM_TIME;
    UINT16 sitId = SIT_SET_AP_SYSTEM_TIME;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 15;
    //BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x42, 0x51, 0x11};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_ap_system_time_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetModemActivityInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_MODEM_ACTIVITY_INFO;
    UINT16 sitId = SIT_OEM_GET_ACTIVITY_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 15;
    //BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x42, 0x51, 0x11};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_get_modem_activity_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetSvn) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SVN;
    UINT16 sitId = SIT_OEM5_SET_SVN;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 15;
    int reqData = 12;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_set_sv_number_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetForbidFrequency) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FORBID_FREQUENCY;
    UINT16 sitId = SIT_OEM_SET_FORBIDDEN_FREQUENCY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 3;
    int reqData[reqDataSize] = {1, 2, 3};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_forbidden_frequency_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetForbidLteBand) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FORBID_LTE_BAND;
    UINT16 sitId = SIT_OEM_SET_FORBIDDEN_LTE_BAND;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 3;
    int reqData[reqDataSize] = {2, 1, 2};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_forbidden_let_band_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetForbidLteTac) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FORBID_LTE_TAC;
    UINT16 sitId = SIT_OEM_SET_FORBIDDEN_LTE_TAC;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 2;
    int reqData[reqDataSize] = {2, 1};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_forbidden_let_tac_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetSelfLogTcpDumpMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SELFLOG_TCP_DUMP_MODE;
    //UINT16 sitId = SIT_OEM_SET_SELFLOG_TCP_DUMP_MODE;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 2;
    int reqData[reqDataSize] = {2, 1};
    // req IPC is built but rsp IPC is not hanlded on purpose. (which is not hanlded in current unit test)
    // so failure RIL rsp comes.
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestMiscOem, oemSetAllDivSetting) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_ALL_DIV_SETTING;
    UINT16 sitId = SIT_OEM_SET_ALL_4RX_DIV_SETTING_DEFAULT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 2;
    //int reqData[reqDataSize] = {2, 1};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetBsrPara) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_BSR_PARA;
    UINT16 sitId = SIT_OEM_SET_BSR_PARA_REQ;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 3;
    int reqData[reqDataSize] = {1, 2, 3};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_bsr_para_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetAheadRre) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_AHEAD_RRE;
    UINT16 sitId = SIT_OEM_SET_AHEAD_RRE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 3;
    int reqData[reqDataSize] = {1, 2, 3};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_ahead_rre_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetDrdv) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_DRDV;
    UINT16 sitId = SIT_OEM_SET_DRDV;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 3;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_drdv_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetForbidNrCell) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FORBID_NR_CELL;
    UINT16 sitId = SIT_OEM_SET_FORBID_NR_CELL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 22;
    BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00,
                                 0x01, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00,
                                 0x01, 0x00, 0x00, 0x00,
                                 '4', '2', '5', '1', '1', '1'};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_forbid_nr_cell_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetCdmaCapaInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_CDMA_CAPABILITY_INFO;
    UINT16 sitId = SIT_OEM_GET_CDMA_CAPABILITY_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 4;
    //BYTE reqData[reqDataSize] = {0x01, 0x00, 0x00, 0x00};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetRadioNode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_RADIO_NODE;
    UINT16 sitId = SIT_GET_RADIO_NODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 256;
    char reqData[reqDataSize] = {0, };
    memset(reqData, 'A', reqDataSize);
    reqData[reqDataSize - 1] = 0;

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_radio_node_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetRadioNodeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemSetRadioNode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_RADIO_NODE;
    UINT16 sitId = SIT_SET_RADIO_NODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 256;
    char reqData[2][reqDataSize] = {{0,}, {0,} };
    memset(reqData[0], 'A', reqDataSize);
    memset(reqData[1], 'B', reqDataSize);
    reqData[0][256-1] = 0;    // null value end
    reqData[1][256-1] = 0;    // null value end

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_radio_node_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetProvisionUpdate) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_PROVISION_UPDATE_REQUEST;
    UINT16 sitId = SIT_GET_VOLTE_PROVISION_UPDATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_volte_provision_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildDummyRawRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemSetProvisionUpdate) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_PROVISION_UPDATE_DONE_REQUEST;
    UINT16 sitId = SIT_SET_VOLTE_PROVISION_UPDATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_volte_provision_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildDummyRawRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMiscOem, oemRadioConfigReset) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_RADIO_CONFIG_RESET;
    UINT16 sitId = SIT_SET_CFG_DEFAULT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_cfg_default_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetPlmnNameTS32table) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_PLMN_NAME_FROM_SE13TABLE;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    short reqData[2] = {1, 1};
    // No IPC build case
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data shall be check

}

TEST_P(SitrilTestMiscOem, oemT32tableDump) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_TS25TABLE_DUMP;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //int reqData = 1;
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data shall be check
}

TEST_P(SitrilTestMiscOem, oemGetMslCode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_MSL_CODE;
    UINT16 sitId = SIT_GET_MSL_CODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //const unsigned int reqDataSize = 10;
    //int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_msl_code_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemMiscTestUnsol) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEN_MISC_TEST_UNSOL;
    //UINT16 sitId = XXXXX;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int testDataSize = 10;
    // sample of SIT_IND_RRC_INFO data
    char testData[testDataSize] = { 0x02, 0x00, 0x47, 0x07, 0x0a, 0x00, 0x00, 0x00, 0x0e, 0x00 };
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestMiscOem, oemSetSrsPwrLimit) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SRS_PWR_LIMIT;
    UINT16 sitId = SIT_OEM_SET_SRS_PWR_LIMIT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int level = 3;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &level, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_srs_pwr_limit_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemSetTraceDumpCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_TRACE_DUMP_CFG;
    UINT16 sitId = SIT_OEM_SET_TRACE_DUMP_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int config = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &config, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_oem_set_trace_dump_cfg_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestMiscOem, oemGetTraceDumpCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_TRACE_DUMP_CFG;
    UINT16 sitId = SIT_OEM_GET_TRACE_DUMP_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetTraceDumpCfg(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
TEST_P(SitrilTestMiscOem, oemSarControlInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SAR_CONTROL_STATE;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_PSENSOR_CONTROL_STATE, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemSarConnectionInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_RF_CONNECTION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_SAR_RF_CONNECTION, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemModemInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_NW_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildModemDataInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_MODEM_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemSelfLogStatusInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SELFLOG_STATUS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_SELFLOG_STATUS, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemCaBwFilterInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_CA_BW_FILTER;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_CA_BANDWIDTH_FILTER, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemNtwEndcCapabilityInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_ENDC_CAPABILITY;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_IND_ENDC_CAPABILITY, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemBaseStationInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_BASESTATION_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_BASESTATION_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMiscOem, oemDroppedMtCallInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_OEM_IND_DROPPED_MT_CALL_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_DROPPED_MT_CALL_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}
