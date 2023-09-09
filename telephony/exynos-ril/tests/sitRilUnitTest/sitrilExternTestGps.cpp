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

#include "sitrilExternTestGps.h"
#include <slsi/ril_client.h>
#include <libsitril-gps/SITRilGps.h>

typedef int (*GpsReqFunction)(void *req_data, int req_data_len, void *callBackFunc, int channel);

void onRspCallbackForGps(unsigned int msgId, int /*status*/, void* /*data*/, size_t /*length*/, unsigned int /*channel*/) {
    //printf("[%s] (%d) Indication comes, size(%u)\n", __FUNCTION__, event, datalen);
    SitrilExternTest *_externTest = SitrilExternTest::GetInstance();
    if (_externTest != NULL) {
        _externTest->setRspMsgId(msgId);
    } else {
        printf("[%s] externTest is null\n", __FUNCTION__);
    }
}

int onIndCallbackForGps(unsigned int msgId, void * /*data*/, size_t /*datalen*/, unsigned int /*socket_id*/) {
    int ret = -1;
    //printf("[%s] (%d) Indication comes, size(%u)\n", __FUNCTION__, event, datalen);
    SitrilExternTest *_externTest = SitrilExternTest::GetInstance();
    if (_externTest != NULL) {
        _externTest->setIndMsgId(msgId);
        ret = 0;
    } else {
        printf("[%s] externTest is null\n", __FUNCTION__);
    }
    return ret;
}

void commonTestFunctionGpsReq(unsigned int msgId,
                                unsigned char *data_req,
                                int data_req_len,
                                char *data_rsp,
                                int data_rsp_len,
                                int channel,
                                GpsReqFunction req_func,
                                SitrilExternTest *sitRilExternTest) {
    int ret =  req_func(data_req, data_req_len, (void *)onRspCallbackForGps, channel);
    EXPECT_EQ(SITRIL_GPS_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = sitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)msgId, reqData->mMsgId);

    sitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, msgId, data_rsp, data_rsp_len, channel);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

SitrilExternApiGps::SitrilExternApiGps() {
    RequestAGPS = nullptr;
    GpsSetFreqAiding = nullptr;
    GpsGetLppSuplEcidInfo = nullptr;
    GpsGetRrlpSuplEcidInfo = nullptr;
    IndGpsMeasurePositionRsp = nullptr;
    IndReleaseGps = nullptr;
    GpsMoLocationReq = nullptr;
    IndGpsMTLocationReq = nullptr;
    GpsGetLppReqServingCellInfo = nullptr;
    LppProvideCapabilitiesInd = nullptr;
    IndLppRequestAssistData = nullptr;
    LppProvideLocationInfoInd = nullptr;
    LppGpsErrorInd = nullptr;
    IndSuplLppDataInfo = nullptr;
    IndSuplNiMessage = nullptr;
    SetGpsSuplNiReady = nullptr;
    SetGanssMeasPosRsp = nullptr;
    SetGpsLockMode = nullptr;
    GetRefLocation = nullptr;
    SetPseudoRangeMeas = nullptr;
    GetCdmaPreciseTimeAidingInfo = nullptr;
    IndGanssApPosCapRsp = nullptr;
    GetGsmExtInfoMsg = nullptr;
    GpsControlPlaneEnable = nullptr;
    GnssLppProfileSet = nullptr;
    GetGpsCdmaFreqAiding = nullptr;
    GpsRetrieveLocInfo = nullptr;
}

void SitrilExternTestGps::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilExternTest = new SitrilExternTest(RIL_EXTERN_GPS, &mApi);
    SitrilExternTest::SetInstance(mSitRilExternTest);

    void *handle = mSitRilExternTest->getHandle();
    if (handle) {
        mApi.ril_open_client              = (void* (*)(const struct RILC_EnvUnitTest *env))dlsym(handle, "GpsOpenUnitTest");
        mApi.ril_close_client             = (int (*)(void))dlsym(handle, "GpsClose");
        mApi.ril_register_callback       = (int (*)(int *))dlsym(handle, "RegisterCallback");
        mApi.RequestAGPS                 = (int (*)(unsigned int, void*, int, void*, int))dlsym(handle, "RequestAGPS");
        mApi.GpsSetFreqAiding             = (int (*)(void*, int, void*, int))dlsym(handle, "GpsSetFreqAiding");
        mApi.GpsGetLppSuplEcidInfo        = (int (*)(void*, int, void*, int))dlsym(handle, "GpsGetLppSuplEcidInfo");
        mApi.GpsGetRrlpSuplEcidInfo       = (int (*)(void*, int, void*, int))dlsym(handle, "GpsGetRrlpSuplEcidInfo");
        mApi.IndGpsMeasurePositionRsp     = (int (*)(void*, int, void*, int))dlsym(handle, "IndGpsMeasurePositionRsp");
        mApi.IndReleaseGps                = (int (*)(void*, int, void*, int))dlsym(handle, "IndReleaseGps");
        mApi.GpsMoLocationReq             = (int (*)(void*, int, void*, int))dlsym(handle, "GpsMoLocationReq");
        mApi.IndGpsMTLocationReq          = (int (*)(void*, int, void*, int))dlsym(handle, "IndGpsMTLocationReq");
        mApi.GpsGetLppReqServingCellInfo  = (int (*)(void*, int, void*, int))dlsym(handle, "GpsGetLppReqServingCellInfo");
        mApi.LppProvideCapabilitiesInd    = (int (*)(void*, int, void*, int))dlsym(handle, "LppProvideCapabilitiesInd");
        mApi.IndLppRequestAssistData      = (int (*)(void*, int, void*, int))dlsym(handle, "IndLppRequestAssistData");
        mApi.LppProvideLocationInfoInd    = (int (*)(void*, int, void*, int))dlsym(handle, "LppProvideLocationInfoInd");
        mApi.LppGpsErrorInd               = (int (*)(void*, int, void*, int))dlsym(handle, "LppGpsErrorInd");
        mApi.IndSuplLppDataInfo           = (int (*)(void*, int, void*, int))dlsym(handle, "IndSuplLppDataInfo");
        mApi.IndSuplNiMessage             = (int (*)(void*, int, void*, int))dlsym(handle, "IndSuplNiMessage");
        mApi.SetGpsSuplNiReady            = (int (*)(void*, int, void*, int))dlsym(handle, "SetGpsSuplNiReady");
        mApi.SetGanssMeasPosRsp           = (int (*)(void*, int, void*, int))dlsym(handle, "SetGanssMeasPosRsp");
        mApi.SetGpsLockMode               = (int (*)(void*, int, void*, int))dlsym(handle, "SetGpsLockMode");
        mApi.GetRefLocation               = (int (*)(void*, int, void*, int))dlsym(handle, "GetRefLocation");
        mApi.SetPseudoRangeMeas           = (int (*)(void*, int, void*, int))dlsym(handle, "SetPseudoRangeMeas");
        mApi.GetCdmaPreciseTimeAidingInfo = (int (*)(void*, int, void*, int))dlsym(handle, "GetCdmaPreciseTimeAidingInfo");
        mApi.IndGanssApPosCapRsp          = (int (*)(void*, int, void*, int))dlsym(handle, "IndGanssApPosCapRsp");
        mApi.GetGsmExtInfoMsg             = (int (*)(void*, int, void*, int))dlsym(handle, "GetGsmExtInfoMsg");
        mApi.GpsControlPlaneEnable        = (int (*)(void*, int, void*, int))dlsym(handle, "GpsControlPlaneEnable");
        mApi.GnssLppProfileSet            = (int (*)(void*, int, void*, int))dlsym(handle, "GnssLppProfileSet");
        mApi.GetGpsCdmaFreqAiding         = (int (*)(void*, int, void*, int))dlsym(handle, "GetGpsCdmaFreqAiding");
        mApi.GpsRetrieveLocInfo           = (int (*)(void*, int, void*, int))dlsym(handle, "GpsRetrieveLocInfo");
    }

    mSitRilExternTest->openClient();
}

void SitrilExternTestGps::TearDown() {
    mSitRilExternTest->closeClient();
    mSitRilExternTest->resetTestData();

    delete mSitRilExternTest;
    mSitRilExternTest = nullptr;
    SitrilExternTest::SetInstance(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////
TEST_P(SitrilExternTestGps, gpsSetFreqAiding) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsSetFreqAiding);

    unsigned int msgId = RILC_REQ_GPS_SET_FREQUENCY_AIDING;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsSetFreqAiding, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsGetLppSuplEcidInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsGetLppSuplEcidInfo);

    unsigned int msgId = RILC_REQ_GPS_GET_LPP_SUPL_REQ_ECID_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsGetLppSuplEcidInfo, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsGetRrlpSuplEcidInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsGetRrlpSuplEcidInfo);

    unsigned int msgId = RILC_REQ_GPS_SET_RRLP_SUPL_REQ_ECID_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsGetRrlpSuplEcidInfo, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indGpsMeasurePositionRsp) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndGpsMeasurePositionRsp);

    unsigned int msgId = RILC_REQ_GPS_MEASURE_POS_RSP;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndGpsMeasurePositionRsp, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indReleaseGps) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndReleaseGps);

    unsigned int msgId = RILC_REQ_GPS_RELEASE_GPS;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndReleaseGps, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsMoLocationReq) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsMoLocationReq);

    unsigned int msgId = RILC_REQ_GPS_MO_LOCATION_REQUEST;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsMoLocationReq, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indGpsMTLocationReq) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndGpsMTLocationReq);

    unsigned int msgId = RILC_REQ_GPS_MT_LOCATION_REQUEST;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndGpsMTLocationReq, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsGetLppReqServingCellInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsGetLppReqServingCellInfo);

    unsigned int msgId = RILC_REQ_GPS_GET_LPP_REQ_SERVING_CELL_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsGetLppReqServingCellInfo, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, lppProvideCapabilitiesInd) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.LppProvideCapabilitiesInd);

    unsigned int msgId = RILC_REQ_GPS_LPP_PROVIDE_CAPABILITIES;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.LppProvideCapabilitiesInd, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indLppRequestAssistData) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndLppRequestAssistData);

    unsigned int msgId = RILC_REQ_GPS_LPP_REQUEST_ASSIST_DATA;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndLppRequestAssistData, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, lppProvideLocationInfoInd) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.LppProvideLocationInfoInd);

    unsigned int msgId = RILC_REQ_GPS_LPP_PROVIDE_LOCATION_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.LppProvideLocationInfoInd, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, lppGpsErrorInd) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.LppGpsErrorInd);

    unsigned int msgId = RILC_REQ_GPS_LPP_GPS_ERROR_IND;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.LppGpsErrorInd, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indSuplLppDataInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndSuplLppDataInfo);

    unsigned int msgId = RILC_REQ_GPS_SUPL_LPP_DATA_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndSuplLppDataInfo, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indSuplNiMessage) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndSuplNiMessage);

    unsigned int msgId = RILC_REQ_GPS_SUPL_NI_MESSAGE;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndSuplNiMessage, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, setGpsSuplNiReady) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.SetGpsSuplNiReady);

    unsigned int msgId = RILC_REQ_GPS_SET_SUPL_NI_READY;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.SetGpsSuplNiReady, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, setGanssMeasPosRsp) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.SetGanssMeasPosRsp);

    unsigned int msgId = RILC_REQ_GPS_SET_GANSS_MEAS_POS_RSP;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.SetGanssMeasPosRsp, mSitRilExternTest);
}


TEST_P(SitrilExternTestGps, setGpsLockMode) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.SetGpsLockMode);

    unsigned int msgId = RILC_REQ_GPS_SET_GPS_LOCK_MODE;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.SetGpsLockMode, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, getRefLocation) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GetRefLocation);

    unsigned int msgId = RILC_REQ_GPS_GET_REFERENCE_LOCATION;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GetRefLocation, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, setPseudoRangeMeas) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.SetPseudoRangeMeas);

    unsigned int msgId = RILC_REQ_GPS_SET_PSEUDO_RANGE_MEASUREMENTS;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.SetPseudoRangeMeas, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, getCdmaPreciseTimeAidingInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GetCdmaPreciseTimeAidingInfo);

    unsigned int msgId = RILC_REQ_GPS_GET_CDMA_PRECISE_TIME_AIDING_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GetCdmaPreciseTimeAidingInfo, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, indGanssApPosCapRsp) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.IndGanssApPosCapRsp);

    unsigned int msgId = RILC_REQ_GPS_GANSS_AP_POS_CAP_RSP;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.IndGanssApPosCapRsp, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, getGsmExtInfoMsg) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GetGsmExtInfoMsg);

    unsigned int msgId = RILC_REQ_GPS_GET_GSM_EXT_INFO_MSG;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GetGsmExtInfoMsg, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsControlPlaneEnable) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsControlPlaneEnable);

    unsigned int msgId = RILC_REQ_GPS_CONTROL_PLANE_ENABLE;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsControlPlaneEnable, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gnssLppProfileSet) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GnssLppProfileSet);

    unsigned int msgId = RILC_REQ_GPS_GNSS_LPP_PROFILE_SET;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GnssLppProfileSet, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, getGpsCdmaFreqAiding) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GetGpsCdmaFreqAiding);

    unsigned int msgId = RILC_REQ_GPS_CDMA_FREQ_AIDING;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GetGpsCdmaFreqAiding, mSitRilExternTest);
}

TEST_P(SitrilExternTestGps, gpsRetrieveLocInfo) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.GpsRetrieveLocInfo);

    unsigned int msgId = RILC_REQ_GPS_RETRIEVE_LOC_INFO;
    unsigned char data_req[10] = {1,2,3,4,};
    int data_req_len = 4;
    char data_rsp[10] = {5, 6, 7, 8, };
    int data_rsp_len = 4;

    commonTestFunctionGpsReq(msgId, data_req, data_req_len, data_rsp, data_rsp_len, mPhoneIndex,
                                mApi.GpsRetrieveLocInfo, mSitRilExternTest);
}

////////////////////////////////////////////////////////////////////////////////////////
TEST_P(SitrilExternTestGps, indProcess) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_register_callback);

    int ret = mApi.ril_register_callback((int *)onIndCallbackForGps);
    ASSERT_EQ(SITRIL_GPS_ERROR_NONE, ret);

    char data_ind[10] = {1, 2, 3, 4, };
    int data_ind_len = 4;

    const int numOfMsgIds = 22;
    int usolMsgId[numOfMsgIds] = {
        0,
        RILC_UNSOL_GPS_MEASURE_POS_REQ,
        RILC_UNSOL_GPS_ASSIST_DATA,
        RILC_UNSOL_GPS_RELEASE_GPS,
        RILC_UNSOL_GPS_MT_LOCATION_REQUEST,
        RILC_UNSOL_GPS_RESET_GPS_ASSIST_DATA,
        RILC_UNSOL_GPS_LPP_REQUEST_CAPABILITIES,
        RILC_UNSOL_GPS_LPP_PROVIDE_ASSIST_DATA,
        RILC_UNSOL_GPS_LPP_REQUEST_LOCATION_INFO,
        RILC_UNSOL_GPS_LPP_GPS_ERROR_IND,
        RILC_UNSOL_GPS_SUPL_LPP_DATA_INFO,
        RILC_UNSOL_GPS_SUPL_NI_MESSAGE,
        RILC_UNSOL_GPS_SUPL_NI_READY,
        RILC_UNSOL_GPS_START_MDT_LOC,
        RILC_UNSOL_GPS_LPP_UPDATE_UE_LOC_INFO,
        RILC_UNSOL_GPS_LOCK_MODE,
        RILC_UNSOL_GPS_3GPP_SEND_GANSS_ASSIT_DATA,
        RILC_UNSOL_GPS_GANSS_MEAS_POS_MSG,
        RILC_UNSOL_GPS_CDMA_GPS_POWER_ON,
        RILC_UNSOL_GPS_CDMA_SEND_ACQUSITION_ASSIT_DATA,
        RILC_UNSOL_GPS_CDMA_SESSION_CANCELLATION,
        RILC_UNSOL_GPS_GANSS_AP_POS_CAP_REQ };

    /* for unsupported msg ID case */
    mSitRilExternTest->setIndMsgId(-1);
    mSitRilExternTest->sendIndication(usolMsgId[0], data_ind, data_ind_len, 0);
    EXPECT_EQ(-1, mSitRilExternTest->getIndMsgId());

    /* for supported msg ID case */
    for (int i = 1; i < numOfMsgIds; ++i) {
        mSitRilExternTest->setIndMsgId(-1);
        mSitRilExternTest->sendIndication(usolMsgId[i], data_ind, data_ind_len, 0);
        EXPECT_EQ(usolMsgId[i], mSitRilExternTest->getIndMsgId());
    }
}
