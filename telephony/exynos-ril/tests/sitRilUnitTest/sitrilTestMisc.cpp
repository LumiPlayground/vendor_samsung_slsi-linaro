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

#include "sitrilTestMisc.h"
#include "testUtil.h"
#include <librilutils/vendor.h>

static UINT8* buildGetBasebandVersionRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_get_baseband_version_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_misc_get_baseband_version_rsp *rsp = (sit_misc_get_baseband_version_rsp *)rspData;
    memcpy(rsp->sw_version, "14", 2);

    return rspData;
}

static UINT8* buildGetSignalStrengthRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_signal_strength_rsp_v2);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildQueryTtyModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_get_tty_mode_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetImeiRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_id_get_imei_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_id_get_imei_rsp *rsp = (sit_id_get_imei_rsp *)rspData;
    rsp->imei_len = 8;
    memcpy(rsp->imei, "12345678", 8);

    return rspData;
}

static UINT8* buildGetImeisvRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_id_get_imeisv_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_id_get_imeisv_rsp *rsp = (sit_id_get_imeisv_rsp *)rspData;
    rsp->imeisv_len = 9;
    memcpy(rsp->imeisv, "123456780", 9);
    return rspData;
}

static UINT8* buildGetDeviceIdRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_id_get_deviceid_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetActivityInfoRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_get_activity_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildStartLceRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_start_lce_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildStopLceRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_stop_lce_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildPullLceRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_get_lce_data_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetCdmaSubscriptionRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_get_cdma_subscription_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static UINT8* buildSigStrengthInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_signal_strength_ind_v2);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    return rspData;
}

static UINT8* buildNitzInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_nitz_time_received_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_nitz_time_received_ind *ind = (sit_misc_nitz_time_received_ind*)rspData;
    ind->daylight_valid = SIT_NITZ_DAYLIGHT_INFO_VALID;
    ind->year = 20;
    ind->month = 12;
    ind->day = 24;

    return rspData;
}

static UINT8* buildPhoneResetInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pwr_phone_reset_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pwr_phone_reset_ind *ind = (sit_pwr_phone_reset_ind*)rspData;
    //When reset_type is 1 ~ 3, there is usleep() call in sitril, it makes seq curruption.
    //So value 4 is used.
    ind->reset_type = 0x4;

    return rspData;
}

static UINT8* buildLceDataInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_lce_data_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    return rspData;
}

static UINT8* buildHardwareConfChangedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_hw_config_change_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_hw_config_change_ind *ind = (sit_misc_hw_config_change_ind*)rspData;
    ind->num_recodrs = 0x1;
    ind->hw_config[0].state = RIL_HARDWARE_CONFIG_STATE_ENABLED;
    ind->hw_config[0].type = RIL_HARDWARE_CONFIG_MODEM;
    memcpy(ind->hw_config[0].uuid, "sitModem0", 9);

    ind->hw_config[0].cfg.modem.max_data = 1;
    ind->hw_config[0].cfg.modem.max_standby = 1;
    ind->hw_config[0].cfg.modem.max_voice = 1;
    ind->hw_config[0].cfg.modem.rat = 0xFFFFF;
    ind->hw_config[0].cfg.modem.ril_model = 1;

    return rspData;
}

static UINT8* buildCdmaPrlChangedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_cdma_prl_change_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_cdma_prl_change_ind *ind = (sit_misc_cdma_prl_change_ind*)rspData;
    ind->prl_ver = 1;

    return rspData;
}

static UINT8* buildModemRestartInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pwr_modem_restart_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pwr_modem_restart_ind *ind = (sit_pwr_modem_restart_ind*)rspData;
    memcpy(ind->reason, "test", 4);

    return rspData;
}

static UINT8* buildRssiScanInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_rssi_scan_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_rssi_scan_ind *ind = (sit_misc_rssi_scan_ind*)rspData;
    ind->total_page = 0;
    ind->current_page = 1;
    ind->start_frequency = 2;
    ind->end_frequency = 3;
    ind->step = 4;
    ind->result[0] = 5;
    ind->result[1] = 6;
    ind->result[2] = 7;
    ind->result[3] = 8;

    return rspData;
}

static UINT8* buildOemPinConrolInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_misc_pin_control_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_pin_control_ind *ind = (sit_misc_pin_control_ind*)rspData;
    ind->signal = 1;
    ind->status = 1;

    return rspData;
}

static UINT8* buildOemSarRfConnectioinInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_rf_connection_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_rf_connection_ind *ind = (sit_rf_connection_ind*)rspData;
    ind->rf_cable_state = 1;

    return rspData;
}

static UINT8* buildAtCommandInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    const char at[] = "\r\n+TEST:1\r\n";

    UINT16 atLen = sizeof(at);
    UINT16 len = sizeof(RCM_IND_HEADER) + sizeof(unsigned short) + atLen;

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_misc_forwarding_at_command_ind *ind = (sit_misc_forwarding_at_command_ind*)rspData;
    ind->length = atLen;
    strncpy(ind->data, at, atLen);

    return rspData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// sitrilTestMisc class
void SitrilTestMisc::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestMisc::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// GET request|response
TEST_P(SitrilTestMisc, getBasebandVersion) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_BASEBAND_VERSION;
    UINT16 sitId = SIT_GET_BASEBAND_VERSION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_baseband_version_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetBasebandVersionRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getSigStrength) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SIGNAL_STRENGTH;
    UINT16 sitId = SIT_GET_SIGNAL_STRENGTH;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_signal_strength_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

     mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetSignalStrengthRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getTtyMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_QUERY_TTY_MODE;
    UINT16 sitId = SIT_GET_TTY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_tty_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildQueryTtyModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setTtyMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_TTY_MODE;
    //UINT16 sitId = XXXXX;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;
    // no IPC build
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setScreenState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SCREEN_STATE;
    UINT16 sitId = SIT_SET_SCREEN_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 1;    // ON
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_screen_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, sendDeviceState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SEND_DEVICE_STATE;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData[2] = {1, 1};    // {RIL_DST_CHARGING_STATE, ON}
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getImei) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_IMEI;
    UINT16 sitId = SIT_GET_IMEI;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_get_imei_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetImeiRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getImeisv) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_IMEISV;
    UINT16 sitId = SIT_GET_IMEISV;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_get_imeisv_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetImeisvRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getDeviceIdentity) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_DEVICE_IDENTITY;
    UINT16 sitId = SIT_GET_DEVICE_ID;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_get_deviceid_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetDeviceIdRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getActivityInfo_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_ACTIVITY_INFO;
    UINT16 sitId = SIT_GET_ACTIVITY_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_activity_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetActivityInfoRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getActivityInfo_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_ACTIVITY_INFO;
    UINT16 sitId = SIT_GET_ACTIVITY_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_activity_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, startLce_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_START_LCE;
    UINT16 sitId = SIT_START_LCE_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData[2] = {10, 0};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_start_lce_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildStartLceRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, startLce_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_START_LCE;
    UINT16 sitId = SIT_START_LCE_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData[2] = {10, 0};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_start_lce_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, stopLce_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_STOP_LCE;
    UINT16 sitId = SIT_STOP_LCE_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_stop_lce_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildStopLceRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, stopLce_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_STOP_LCE;
    UINT16 sitId = SIT_STOP_LCE_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_stop_lce_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, pullLceData_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_PULL_LCEDATA;
    UINT16 sitId = SIT_GET_LCE_DATA;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_get_lce_data_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildPullLceRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, pullLceData_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_PULL_LCEDATA;
    UINT16 sitId = SIT_GET_LCE_DATA;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_get_lce_data_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getCdmaSubSrc) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setCdmaSubSrc_nv) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 1;    // CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setCdmaSubSrc_ruim_sim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getCdmaSubScription_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SUBSCRIPTION;
    UINT16 sitId = SIT_GET_CDMA_SUBSCRIPTION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_cdma_subscription_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetCdmaSubscriptionRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData= psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getCdmaSubScription_e_not_available) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SUBSCRIPTION;
    UINT16 sitId = SIT_GET_CDMA_SUBSCRIPTION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_cdma_subscription_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData= psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getHardwareConf) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_HARDWARE_CONFIG;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setVoiceOp_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_VOICE_OPERATION;
    UINT16 sitId = SIT_SET_VOICE_OPERATION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // CS only
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_voice_operation_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setVoiceOp_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_VOICE_OPERATION;
    UINT16 sitId = SIT_SET_VOICE_OPERATION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // CS only
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_voice_operation_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setUnsolFilter) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER;
    UINT16 sitId = SIT_SET_IND_CMD_FILTER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // RIL_IndicationFilter bit flag
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_ind_cmd_filter_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getNcellId) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_NEIGHBORING_CELL_IDS;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setLocUpdates_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_LOCATION_UPDATES;
    UINT16 sitId = SIT_SET_LOCATION_UPDATE_SETTING;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // diabled
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_location_update_setting_req);
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

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setLocUpdates_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_LOCATION_UPDATES;
    UINT16 sitId = SIT_SET_LOCATION_UPDATE_SETTING;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 0;    // diabled
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_location_update_setting_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setSuppSvcNotification) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION;
    //UINT16 sitId = 0;    // no IPC definition
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Temp: There is no IPC
    int reqData = 0;    // diabled
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setDataThrottling) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_DATA_THROTTLING;
    UINT16 sitId = SIT_SET_DATA_THROTTLING;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_DataThrottling reqData = { NO_DATA_THROTTLING, 10 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_data_throttling_req);
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

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, getSystemSelectionChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS;
    UINT16 sitId = SIT_GET_SYSTEM_SELECTION_CHANNELS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RCM_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_GE((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setCarrierInfoImsiEncryption) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION;
    UINT16 sitId = SIT_SET_CARRIER_INFO_IMSI_ENCRYPTION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_CarrierInfoForImsiEncryption reqData = {nullptr, nullptr, nullptr, 0, nullptr, 0};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_set_carrier_info_imsi_encription_req);
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

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setSignalStrengthReportingCriteria) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA;
    int encodedTestRilCmdId = ENCODE_REQUEST(testRilCmdId, HAL_VERSION_CODE(1,5));
    UINT16 sitId = SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const uint32_t numOfThresholds = 4;
    int arrayInValidData[numOfThresholds] = {-109, -103, -97, -89};
    int arrayValidData[numOfThresholds] = {-110, -99, -88, - 77};
    RIL_SignalStrengthReportingCriteria_V1_5 reqData = {};
    reqData.accessNetwork = 1;    // RIL_AccessNetwork, RIL_AccessNetwork_V1_5
    reqData.signalThresholdInfo.signalMeasurement = RSSI;    //(RIL_SignalMeasurementType)
    reqData.signalThresholdInfo.hysteresisMs = 1;
    reqData.signalThresholdInfo.hysteresisDb = 10;
    reqData.signalThresholdInfo.len_thresholds = numOfThresholds;
    reqData.signalThresholdInfo.thresholds =  arrayInValidData;
    reqData.signalThresholdInfo.isEnabled = false;
    // IPC No build case
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), encodedTestRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    /* for next request, rest data */
    psitRilTest->resetTestData();
    reqData.signalThresholdInfo.thresholds = arrayValidData;
    reqData.signalThresholdInfo.isEnabled = true;
    // IPC build case
    psitRilTest->processReq(GetRandomSerialNumber(), encodedTestRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_signal_strength_report_criteria_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(encodedTestRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setLinkCapacityReportingCriteria) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA;
    int encodedTestRilCmdId = ENCODE_REQUEST(testRilCmdId, HAL_VERSION_CODE(1,5));
    UINT16 sitId = SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const uint32_t numOfThresholds = 4;
    unsigned int arrayInValidData[numOfThresholds] = {1000, 1500, 3000, 5000};
    unsigned int arrayValidData[numOfThresholds] = {1000, 3000, 5000, 7000};
    RIL_LinkCapacityReportingCriteria_V1_5 reqData = {};
    reqData.hysteresisMs = 10;
    reqData.hysteresisDlKbps = 1000;
    reqData.hysteresisUlKbps = 1000;
    reqData.numOfThresholdsDownlinkKbps = numOfThresholds;
    reqData.thresholdsDownlinkKbps = arrayInValidData;
    reqData.numOfThresholdsUplinkKbps = numOfThresholds;
    reqData.thresholdsUplinkKbps = arrayInValidData;
    reqData.accessNetwork = ACCESS_NETWORK_NGRAN;
    // IPC No build case
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), encodedTestRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    /* for next request, reset data */
    psitRilTest->resetTestData();
    reqData.thresholdsDownlinkKbps = arrayValidData;
    // IPC No build case
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), encodedTestRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    /* for next request, rest data */
    psitRilTest->resetTestData();
    reqData.thresholdsUplinkKbps = arrayValidData;
    // IPC build case
    psitRilTest->processReq(GetRandomSerialNumber(), encodedTestRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_link_capacity_report_criteria_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(encodedTestRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, readNvItem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_NV_READ_ITEM;
    //UINT16 sitId = XXX;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_NV_ReadItem testVector = { RIL_NV_CDMA_MEID };
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    // It returns failure because it is not supported.
    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestMisc, writeNvItem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_NV_WRITE_ITEM;
    //UINT16 sitId = XXX;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    char testData[10] = "test";
    RIL_NV_WriteItem testVector = { RIL_NV_CDMA_MEID, testData};
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    // It returns failure because it is not supported.
    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}


TEST_P(SitrilTestMisc, getPhoneConfig) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_PHONE_CAPABILITY;    // MSG_MISC_GET_PHONE_CAPABILITY
    UINT16 sitId = SIT_GET_PHONE_CAPABILITY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RCM_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, setModemConfig) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_MODEMS_CONFIG;    // MSG_MISC_SET_MODEMS_CONFIG
    UINT16 sitId = SIT_SET_MODEM_CONFIG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // No IPC case
    int testVector = 3;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);
    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    psitRilTest->resetTestData();

    // IPC case
    testVector = 2;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);
    EXPECT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_modems_config_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RCM_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, oemHookRaw) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_HOOK_RAW;
    //UINT16 sitId = 0;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    // There is no service for hanlding this RIL req in current, so RIL_E_REQUEST_NOT_SUPPORTED will comes
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestMisc, oemSetPinControl) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_PIN_CONTROL;
    UINT16 sitId = SIT_SET_PIN_CONTROL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    BYTE reqData[2] = { 1, 2 };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_pin_control_req);
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

TEST_P(SitrilTestMisc, oemSendSgc) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SEND_SGC;
    UINT16 sitId = SIT_SET_SGC;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_set_sgc_req);
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

TEST_P(SitrilTestMisc, oemSetFeatureInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FEATURE_INFO;
    UINT16 sitId = SIT_SET_FEATURE_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //int reqData = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_feature_info_req);
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

TEST_P(SitrilTestMisc, oemSetEngMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_ENG_MODE;
    UINT16 sitId = SIT_SET_ENG_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    char reqData = '0';
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_eng_mode_req);
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

TEST_P(SitrilTestMisc, oemSetScreenLine) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_SCR_LINE;
    UINT16 sitId = SIT_SET_SCREEN_LINE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    char reqData = '0';
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_scr_line_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

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

TEST_P(SitrilTestMisc, oemSetEngStringInput) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_ENG_STRING_INPUT;
    UINT16 sitId = SIT_SET_ENG_STRING_INPUT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 10;
    char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, reqDataSize, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + reqDataSize + 1;
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

TEST_P(SitrilTestMisc, oemSetDbgTrace) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_DEBUG_TRACE;
    UINT16 sitId = SIT_SET_DEBUG_TRACE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 1;
    char reqData[reqDataSize+1] = "0";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, reqDataSize, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_debug_trace_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

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

TEST_P(SitrilTestMisc, oemForwardAtCommand) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_FORWARDING_AT_COMMAND;
    UINT16 sitId = SIT_SET_FORWARDING_AT_COMMAND;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const unsigned int reqDataSize = 10;
    char reqData[reqDataSize+1] = "0123456789";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, reqData, reqDataSize, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + sizeof(unsigned short) + reqDataSize;
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

TEST_P(SitrilTestMisc, oemSetTtyMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_TTY_MODE;
    UINT16 sitId = SIT_SET_TTY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int reqData = 1;    // full = RIL_TTY_MODE_FULL
    // No IPC build case
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_tty_mode_req);
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

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
TEST_P(SitrilTestMisc, sigStrengthInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SIGNAL_STRENGTH;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildSigStrengthInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_SIGNAL_STRENGTH, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, nitzTimeInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NITZ_TIME_RECEIVED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildNitzInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NITZ_TIME_RECEIVED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, phoneResetInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_PHONE_RESET;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildPhoneResetInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_AM, mRspData, mRspDataLen);

    // timeout is expected. because there is no RIL unsol
    ASSERT_EQ(std::cv_status::timeout, psitRilTest->wait());
}

TEST_P(SitrilTestMisc, lcdDataInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_LCE_DATA;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildLceDataInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_LCEDATA_RECV, mRspData, mRspDataLen);

    // It is not implemented, so RIL unsol will not be sent.
}

TEST_P(SitrilTestMisc, carrierInfoImsiEncryptInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CARRIER_INFO_IMSI_ENCRYPTION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildEmptyInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, hwConfChangedInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_HW_CONFIG_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildHardwareConfChangedInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_HARDWARE_CONFIG_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, cdmaPrlChangedInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_PRL_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildCdmaPrlChangedInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CDMA_PRL_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, modemRestartInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_MODEM_RESTART;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildModemRestartInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_MODEM_RESTART, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, rssiScanInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RSSI_SCAN;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRssiScanInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_SCAN_RSSI_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);

}

TEST_P(SitrilTestMisc, oemPinCotrolInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_PIN_CONTROL;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildOemPinConrolInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_PIN_CONTROL, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, oemSarRfConnectionInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RF_CONNECTION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildOemSarRfConnectioinInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_SAR_RF_CONNECTION, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, oemDisplayInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_DISPLAY_ENG;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_DISPLAY_ENG_MODE, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestMisc, oemAtCommandInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_FORWARDING_AT_COMMAND;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildAtCommandInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_FORWARDING_AT_COMMAND, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}
