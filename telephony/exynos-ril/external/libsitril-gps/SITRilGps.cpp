/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define LOG_TAG "LibSITRil-Gps"
#include <utils/Log.h>

#include "rilclienthelper.h"
#include "sitril-client.h"
#include "SITRilGps.h"
#include "SITRilGpsDef.h"

// ####  Logs Seleection ####
//#define ENABLE_LOGS_FUNC_ENTER_EXIT
#define ENABLE_ANDROID_LOG
#define LogE    ALOGE
#define LogW    ALOGW
#define LogN    ALOGI
#ifdef ENABLE_ANDROID_LOG
#define LogI    ALOGI
#define LogV    ALOGI
#endif // end  of ENABLE_ANDROID_LOG

// #### Defintions ####

#define ENTER_FUNC()        { LogV("%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()        { LogV("%s() [--> ", __FUNCTION__); }

// #### Global Variables ####
void *gSITRILC_Handle = NULL;
RilClientHelper *g_RilClientHelper = NULL;
Rilc_OnResponse mUnitTestSolicitedResponse = nullptr;    // for unit test

int (*gSITRilGpsUnsolCallback)(unsigned msgId, void *data, size_t datalen, unsigned int channel);

// #### Internal Functions ####

void SITRilClient_OnResponse(unsigned msgId, int status, void* data, size_t length, unsigned int channel)
{
    ENTER_FUNC();
    LogV("%s(msgId(%d), status(%d), data(0x%p), length(%zu), channel(%d)++",__FUNCTION__,msgId,status,data,length,channel);

    switch(msgId)
    {
        /*
        //example
        //case RILC_REQ_GPS_SET_FREQ_AIDING:
        //    LogN("%s() msgId(%d) is sent",__FUNCTION__,msgId);
        //    break;
        */

        default:
            LogV("%s() Unkonwn msgId = %d",__FUNCTION__,msgId);
            break;
    }

    if (mUnitTestSolicitedResponse) mUnitTestSolicitedResponse(msgId, status, data, length, channel);

    LEAVE_FUNC();
}

void SITRilClient__OnUnsolicitedResponse(unsigned msgId, void* data, size_t length, unsigned int channel)
{
    ENTER_FUNC();

    LogV("%s() msgId(%d), data(0x%p), length(%zu)",__FUNCTION__,msgId,data,length);

    do
    {
        if(gSITRilGpsUnsolCallback == NULL)
        {
            LogE("%s() gSITRilGpsUnsolCallback == NULL",__FUNCTION__);
            break;
        }

        // Check GPS msgId
        switch(msgId)
        {
            case RILC_UNSOL_GPS_MEASURE_POS_REQ:
                LogV("%s() RILC_UNSOL_GPS_MEASURE_POS_REQ = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_ASSIST_DATA:
                LogV("%s() RILC_UNSOL_GPS_ASSIST_DATA = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_RELEASE_GPS:
                LogV("%s() RILC_UNSOL_GPS_RELEASE_GPS = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_MT_LOCATION_REQUEST:
                LogV("%s() RILC_UNSOL_GPS_MT_LOCATION_REQUEST = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_RESET_GPS_ASSIST_DATA:
                LogV("%s() RILC_UNSOL_GPS_RESET_GPS_ASSIST_DATA = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_LPP_REQUEST_CAPABILITIES:
                LogV("%s() RILC_UNSOL_GPS_LPP_REQUEST_CAPABILITIES = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_LPP_PROVIDE_ASSIST_DATA:
                LogV("%s() RILC_UNSOL_GPS_LPP_PROVIDE_ASSIST_DATA = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_LPP_REQUEST_LOCATION_INFO:
                LogV("%s() RILC_UNSOL_GPS_LPP_REQUEST_LOCATION_INFO = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_LPP_GPS_ERROR_IND:
                LogV("%s() RILC_UNSOL_GPS_LPP_GPS_ERROR_IND = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_SUPL_LPP_DATA_INFO:
                LogV("%s() RILC_UNSOL_GPS_SUPL_LPP_DATA_INFO = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_SUPL_NI_MESSAGE:
                LogV("%s() RILC_UNSOL_GPS_SUPL_NI_MESSAGE = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_3GPP_SEND_GANSS_ASSIT_DATA:
                LogV("%s() RILC_UNSOL_GPS_3GPP_SEND_GANSS_ASSIT_DATA = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_GANSS_MEAS_POS_MSG:
                LogV("%s() RILC_UNSOL_GPS_GANSS_MEAS_POS_MSG = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_CDMA_GPS_POWER_ON:
                LogV("%s() RILC_UNSOL_GPS_CDMA_GPS_POWER_ON = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_CDMA_SEND_ACQUSITION_ASSIT_DATA:
                LogV("%s() RILC_UNSOL_GPS_CDMA_SEND_ACQUSITION_ASSIT_DATA = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_CDMA_SESSION_CANCELLATION:
                LogV("%s() RILC_UNSOL_GPS_CDMA_SESSION_CANCELLATION = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_GANSS_AP_POS_CAP_REQ:
                LogV("%s() RILC_UNSOL_GPS_GANSS_AP_POS_CAP_REQ = %d",__FUNCTION__,msgId);
                break;
            case RILC_UNSOL_GPS_SUPL_NI_READY:
                LogV("%s() RILC_UNSOL_GPS_SUPL_NI_READY = %d", __FUNCTION__, msgId);
                break;
            case RILC_UNSOL_GPS_START_MDT_LOC:
                LogV("%s() RILC_UNSOL_GPS_START_MDT_LOC = %d", __FUNCTION__, msgId);
                break;
            case RILC_UNSOL_GPS_LPP_UPDATE_UE_LOC_INFO:
                LogV("%s() RILC_UNSOL_GPS_LPP_UPDATE_UE_LOC_INFO = %d", __FUNCTION__, msgId);
                break;
            case RILC_UNSOL_GPS_LOCK_MODE:
                LogV("%s() RILC_UNSOL_GPS_LOCK_MODE = %d", __FUNCTION__, msgId);
                break;
            case RILC_UNSOL_GPS_CAMPING_ON_LTE:
                LogV("%s() RILC_UNSOL_GPS_CAMPING_ON_LTE = %d", __FUNCTION__, msgId);
                break;
            default:
                LogV("%s() Unkonwn msgId = %d",__FUNCTION__,msgId);
                return;
        }

        // Call Callback Function
        gSITRilGpsUnsolCallback(msgId, data, length, channel);

    }while(0);

    LEAVE_FUNC();
}

void SetIndicationFilter()
{
    int filters[] = {
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
        RILC_UNSOL_GPS_3GPP_SEND_GANSS_ASSIT_DATA,
        RILC_UNSOL_GPS_GANSS_MEAS_POS_MSG,
        RILC_UNSOL_GPS_CDMA_GPS_POWER_ON,
        RILC_UNSOL_GPS_CDMA_SEND_ACQUSITION_ASSIT_DATA,
        RILC_UNSOL_GPS_CDMA_SESSION_CANCELLATION,
        RILC_UNSOL_GPS_GANSS_AP_POS_CAP_REQ,
        RILC_UNSOL_GPS_SUPL_NI_READY,
        RILC_UNSOL_GPS_START_MDT_LOC,
        RILC_UNSOL_GPS_LPP_UPDATE_UE_LOC_INFO,
        RILC_UNSOL_GPS_LOCK_MODE,
        RILC_UNSOL_GPS_CAMPING_ON_LTE,
    };
    size_t size = sizeof(filters) / sizeof(filters[0]);
    if (g_RilClientHelper != NULL) {
        g_RilClientHelper->SetIndicationFilter(gSITRILC_Handle, filters, size);
    }
}

/* ----------------------------------------------------
// Description
//     : Close Gps Interaface of SIT Ril.
// Results
//    - int error value of SITRilGpsError
// ----------------------------------------------------*/
int GpsClose()
{
    int ret = SITRIL_GPS_ERROR_NONE;

    ENTER_FUNC();

    if (g_RilClientHelper != NULL) {
        g_RilClientHelper->SetUnitTest(gSITRILC_Handle, NULL);
        g_RilClientHelper->Close(gSITRILC_Handle);
        LogI("%s() gSITRILC_Close() ... OK.", __FUNCTION__);
    }

    mUnitTestSolicitedResponse = nullptr;    /* for unit test */

    LEAVE_FUNC();

    return ret;
}

/* ----------------------------------------------------
// Description
//     : Open Gps Interaface of SIT Ril.
// Results
//    - int error value of SITRilGpsError
// ----------------------------------------------------*/
int GpsOpen()
{
    int ret = SITRIL_GPS_ERROR_NONE;
    ENTER_FUNC();

    do
    {
        g_RilClientHelper = RilClientHelper::GetInstance();
        if (g_RilClientHelper == NULL) {
            ret = SITRIL_GPS_ERROR_NO_LIB_OPEN_FAIL;
            break;
        }

        // Open
        gSITRILC_Handle = g_RilClientHelper->Open("GpsApi");
        if(gSITRILC_Handle == NULL) {
            LogE("%s() Fail in gSITRILC_Open()", __FUNCTION__);
            ret = SITRIL_GPS_ERROR_NO_DEVICE;
            break;
        }
        LogV("%s() gSITRILC_Open() ... OK.",__FUNCTION__);

        // Register indications to be received
        SetIndicationFilter();

        // Register Callback
        ret = g_RilClientHelper->RegisterUnsolicitedResponseHandler(gSITRILC_Handle, SITRilClient__OnUnsolicitedResponse);
        if(ret)
        {
            LogE("%s() Fail in gSITRILC_Open()",__FUNCTION__);
            ret = SITRIL_GPS_ERROR_REGISTERATION_FAIL;
            break;
        }

        LogV("%s() gSITRILC_Open() ... OK.",__FUNCTION__);


    }while(0);

    if(ret != SITRIL_GPS_ERROR_NONE)
    {
        GpsClose();
    }

    LEAVE_FUNC();

    return ret;
}

/* ----------------------------------------------------
// Description
//     : Open Gps Interaface of SIT Ril for unit test.
// Results
//    - RILC_UnitTestFunctions*, otherwise null ptr
// ----------------------------------------------------*/
const void *GpsOpenUnitTest(const struct RILC_EnvUnitTest *env)
{
    static RILC_UnitTestFunctions test;

    int ret = GpsOpen();

    if (ret == SITRIL_GPS_ERROR_NONE && g_RilClientHelper != NULL) {
        mUnitTestSolicitedResponse = env->OnRequestComplete;
        g_RilClientHelper->SetUnitTest(gSITRILC_Handle, env->OnRequestSend);
    }

    test.openStatus = ret;
    test.onIndSend = SITRilClient__OnUnsolicitedResponse;
    test.onRspSend = SITRilClient_OnResponse;

    return &test;
}

int RequestAGPS(unsigned int msgId, unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    int ret = SITRIL_GPS_ERROR_NONE;

    ENTER_FUNC();
    LogI("%s: msgId=%d data=%p data_len=%d callback=%p", __FUNCTION__, msgId, data, data_len, callback);

    if(gSITRILC_Handle == NULL)
    {
        LogE("%s() gSITRILC_Handle == NULL",__FUNCTION__);
        ret = SITRIL_GPS_ERROR_NO_LIB_OPEN_FAIL;
        goto exit;
    }

    // Sending A message
    ret = g_RilClientHelper->Send(gSITRILC_Handle, msgId, (void*)(data), data_len, callback, channel);
    if(ret)
    {
        LogE("%s() Fail in gSITRILC_Send() ",__FUNCTION__);
        ret = SITRIL_GPS_ERROR_NO_LIB_SEND_FAIL;
        goto exit;
    }

exit:
    LEAVE_FUNC();
    return ret;
}

/* ----------------------------------------------------
// Description
//     : Register Callback function to receive Unsolicited events of SITRil states for Gps
// Results
//    - int error value of SITRilGpsError
// ----------------------------------------------------*/
int RegisterCallback(SITRilGpsUnsolCallback callback)
{
    int ret = SITRIL_GPS_ERROR_NONE;

    ENTER_FUNC();

    do
    {
        if(gSITRilGpsUnsolCallback != NULL)
        {
            LogE("%s() gSITRilGpsCallback(%p)", __FUNCTION__, gSITRilGpsUnsolCallback);
            ret = SITRIL_GPS_ERROR_ALREADY_REGISTERD;
            break;
        }

        gSITRilGpsUnsolCallback = callback;
        LogV("%s() new gSITRilGpsCallback(%p)", __FUNCTION__, gSITRilGpsUnsolCallback);

    }while(0);

    LEAVE_FUNC();

    return ret;
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_SET_GPS_FREQUENCY_AIDING
// RCM ID : 0x0C00
// ----------------------------------------------------*/
int GpsSetFreqAiding(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_FREQUENCY_AIDING, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_LPP_SUPL_REQ_ECID_INFO
// RCM ID : 0x0C01
// ----------------------------------------------------*/
int GpsGetLppSuplEcidInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GET_LPP_SUPL_REQ_ECID_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_RRLP_SUPL_REQ_ECID_INFO
// RCM ID : 0x0C02
// ----------------------------------------------------*/
int GpsGetRrlpSuplEcidInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_RRLP_SUPL_REQ_ECID_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_GPS_MEASURE_POSITION_RSP
// RCM ID : 0x0C04
// ----------------------------------------------------*/
int IndGpsMeasurePositionRsp(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_MEASURE_POS_RSP, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_RELEASE_GPS
// RCM ID : 0x0C06
// ----------------------------------------------------*/
int IndReleaseGps(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_RELEASE_GPS, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GPS_MO_LOCATION_REQUEST
// RCM ID : 0x0C07
// ----------------------------------------------------*/
int GpsMoLocationReq(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_MO_LOCATION_REQUEST, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_GPS_MT_LOCATION_REQUEST
// RCM ID : 0x0C08
//  ----------------------------------------------------*/
int IndGpsMTLocationReq(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_MT_LOCATION_REQUEST, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_LPP_REQ_SERVING_CELL_INFO
// RCM ID : 0x0C0A
// ----------------------------------------------------*/
int GpsGetLppReqServingCellInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GET_LPP_REQ_SERVING_CELL_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_LPP_PROVIDE_CAPABILITIES_IND
// RCM ID : 0x0C0C
// ----------------------------------------------------*/
int LppProvideCapabilitiesInd(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_LPP_PROVIDE_CAPABILITIES, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_LPP_REQUEST_ASSIST_DATA
// RCM ID : 0x0C0D
// ----------------------------------------------------*/
int IndLppRequestAssistData(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_LPP_REQUEST_ASSIST_DATA, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_LPP_PROVIDE_LOCATION_INFO_IND
// RCM ID : 0x0C10
// ----------------------------------------------------*/
int LppProvideLocationInfoInd(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_LPP_PROVIDE_LOCATION_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_LPP_GPS_ERROR_IND
// RCM ID : 0x0C11
// Direction : From AP to NW
// ----------------------------------------------------*/
int LppGpsErrorInd(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_LPP_GPS_ERROR_IND, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_SUPL_LPP_DATA_INFO
// RCM ID : 0x0C12
// Direction : From AP to NW
// ----------------------------------------------------*/
int IndSuplLppDataInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SUPL_LPP_DATA_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_SUPL_NI_MESSAGE
// RCM ID : 0x0C13
// Direction : From AP to NW
// Description
//     : Get Serving Cell Information Response
// ----------------------------------------------------*/
int IndSuplNiMessage(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SUPL_NI_MESSAGE, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_SET_GPS_SUPL_NI_READY
// RCM ID : 0x0C14
// ----------------------------------------------------*/
int SetGpsSuplNiReady(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_SUPL_NI_READY, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_SET_GANSS_MEAS_POS_RSP
// RCM ID : 0x0C17
// Description
//     : This message is used to send Measure Position Response CP/server.
// ----------------------------------------------------*/
int SetGanssMeasPosRsp(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_GANSS_MEAS_POS_RSP, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_SET_GPS_LOCK_MODE
// RCM ID : 0x0C20
// Description
//     : This message is used to set GPS lock mode based on the user setting to enable/disable location services.
//       When GPS lock is enabled,CP shall reject all NI-LR requests from server except for emergency call.
// ----------------------------------------------------*/
int SetGpsLockMode(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_GPS_LOCK_MODE, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_REFERENCE_LOCATION
// RCM ID : 0x0C21
// Description
//     : This message is used to fetch Reference location assistance data from server over TIA-801(CDMA) protocol.
// ----------------------------------------------------*/
int GetRefLocation(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GET_REFERENCE_LOCATION, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_SET_PSEUDO_RANGE_MEASUREMENTS
// RCM ID : 0x0C23
// Description
//     : This message is used to receive Pseudo Range Measurements from GPS chip.
// ----------------------------------------------------*/
int SetPseudoRangeMeas(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_SET_PSEUDO_RANGE_MEASUREMENTS, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_CDMA_PRECISE_TIME_AIDING_INFO
// RCM ID : 0x0C26
// Description
//     : This message is  used to get precise time aiding info from modem.
// ----------------------------------------------------*/
int GetCdmaPreciseTimeAidingInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GET_CDMA_PRECISE_TIME_AIDING_INFO, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_IND_GANSS_AP_POS_CAP_RSP
// RCM ID : 0x0C19
// Direction : from AP
// Description
//     : This message is used by AP to send GSM Position Capability Response to CP.
// ----------------------------------------------------*/
int IndGanssApPosCapRsp(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GANSS_AP_POS_CAP_RSP, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_GSM_EXT_INFO_MSG
// RCM ID : 0x0C1A
// Description
//     : This message is used to get GSM extended Radio signal information.
// ----------------------------------------------------*/
int GetGsmExtInfoMsg(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GET_GSM_EXT_INFO_MSG, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GPS_CONTROL_PLANE_ENABLE
// RCM ID : 0x0C1B
// ----------------------------------------------------*/
int GpsControlPlaneEnable(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_CONTROL_PLANE_ENABLE, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GNSS_LPP_PROFILE_SET
// RCM ID : 0x0C1C
// Description
//     : This message is used to enable/disable LPP features at CP.
// ----------------------------------------------------*/
int GnssLppProfileSet(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_GNSS_LPP_PROFILE_SET, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GET_GPS_CDMA_FREQ_AIDING
// RCM ID : 0x0C27
// Description
//     : This message is used to receive TCXO clock frequency data from CP to help GPS operation.
// ----------------------------------------------------*/
int GetGpsCdmaFreqAiding(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_CDMA_FREQ_AIDING, data, data_len, callback, channel);
}

/* ----------------------------------------------------
// SIT CMD Name : SIT_GPS_RETRIEVE_LOC_INFO
// RCM ID : 0x0C2A
// Description
//     : This message is used to send location information from UE to network.
// ----------------------------------------------------*/
int GpsRetrieveLocInfo(unsigned char *data, int data_len, SITRilGpsOnResponse callback, int channel)
{
    return RequestAGPS(RILC_REQ_GPS_RETRIEVE_LOC_INFO, data, data_len, callback, channel);
}
