/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * oem_deviceinfo.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#include "miscdatabuilder.h"
#include "oem_deviceinfo.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"
#include "service.h"
#include <librilutils/systemproperty.h>

#define TIMEOUT_OEM_DEVICE_INFO_DEFAULT     5000

/**
 * GetRfDesenseModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetRfDesenseModeHandler, MSG_MISC_OEM_GET_RF_DESENSE_MODE)

int GetRfDesenseModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.GetRfDesenseMode();
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetRfDesenseModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(errorCode, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetRfDesenseModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetRfDesenseModeHandler, MSG_MISC_OEM_SET_RF_DESENSE_MODE)

int SetRfDesenseModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.SetRfDesenseMode(rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetRfDesenseModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscSetRfDesenseModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        char cause = adapter.GetCause();
        OnRequestComplete(errorCode, &cause, sizeof(cause));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * GetManualBandModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetManualBandModeHandler, MSG_MISC_OEM_GET_MANUAL_BAND_MODE)

int GetManualBandModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.GetManualBandMode();
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetManualBandModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(errorCode, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetManualBandModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetManualBandModeHandler, MSG_MISC_OEM_SET_MANUAL_BAND_MODE)

int SetManualBandModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.SetManualBandMode(rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetManualBandModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscSetManualBandModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        char cause = adapter.GetCause();
        OnRequestComplete(errorCode, &cause, sizeof(cause));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetPSensorStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPSensorStatusHandler, MSG_MISC_SET_PSENSOR_STATUS)

int SetPSensorStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int pSensorStatus = rildata->GetInt();
    RilLogV("[%s] %s(), P-Sensor status = %d", mServiceName, __FUNCTION__, pSensorStatus);

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.BuildPSensorStatus(pSensorStatus);
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetPSensorStatusHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetSarStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSarStateHandler, MSG_MISC_SET_SAR_STATE)

int SetSarStateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int sarState = rildata->GetInt();
    RilLogV("[%s] %s(), sar state = %d", mServiceName, __FUNCTION__, sarState);

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.BuildSetSarState(sarState);
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetSarStateHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetSarStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetSarStateHandler, MSG_MISC_GET_SAR_STATE)

int GetSarStateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.BuildGetSarState();
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetSarStateHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscGetSarStateAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int sarState = adapter.GetSarState();
        OnRequestComplete(RIL_E_SUCCESS, &sarState, sizeof(sarState));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * UnsolSarControlStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolSarControlStateHandler, MSG_MISC_SAR_CONTROL_STATE_IND)

int UnsolSarControlStateHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscSarControlStateAdapter adapter(msg->GetModemData());
    BYTE device_state = adapter.GetDeviceState();

    RilLogV("[%s] %s() : Device state:%d", mServiceName, __FUNCTION__, device_state);
    OnUnsolicitedResponse(RIL_UNSOL_OEM_PSENSOR_CONTROL_STATE, &device_state, 1);

    return 0;
}

/**
 * UnsolSarRfConnectionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolSarRfConnectionHandler, MSG_MISC_SAR_RF_CONNECTION_IND)

int UnsolSarRfConnectionHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscSarRfConnectionAdapter adapter(pModemData);
    BYTE rf_state = adapter.GetRfState();

    RilLogV("[%s] %s() : RF connection: %d -> %d", mServiceName, __FUNCTION__, SystemProperty::GetInt(RIL_VENDOR_RF_CONNECTION, 0), rf_state);

    if (rf_state > 0) SystemProperty::Set(RIL_VENDOR_RF_CONNECTION, "1");
    else SystemProperty::Set(RIL_VENDOR_RF_CONNECTION, "0");

    OnUnsolicitedResponse(RIL_UNSOL_OEM_SAR_RF_CONNECTION, &rf_state, 1);

    return 0;
}

/**
 * RssiScanHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RssiScanHandler, MSG_MISC_SCAN_RSSI)

int RssiScanHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    const int INDEX_RAT = 0;
    const int INDEX_BAND = 1;
    const int INDEX_RBW = 2;
    const int INDEX_SCAN_MODE = 3;
    const int INDEX_START_FREQUENCY = 4;
    const int INDEX_END_FREQUENCY = 5;
    const int INDEX_STEP = 6;
    const int INDEX_ANTENNA_SELECTION = 7;
    const int INDEX_SAMPLING_COUNT = 8;
    const int INDEX_TX1_ENABLED = 9;
    const int INDEX_TX1_BAND = 10;
    const int INDEX_TX1_BW = 11;
    const int INDEX_TX1_FREQUENCY = 12;
    const int INDEX_TX1_POWER = 13;
    const int INDEX_TX1_RB_NUM = 14;
    const int INDEX_TX1_RB_OFFSET = 15;
    const int INDEX_TX1_MCS = 16;
    const int INDEX_TX2_ENABLED = 17;
    const int INDEX_TX2_BAND = 18;
    const int INDEX_TX2_BW = 19;
    const int INDEX_TX2_FREQUENCY = 20;
    const int INDEX_TX2_POWER = 21;
    const int INDEX_TX2_RB_NUM = 22;
    const int INDEX_TX2_RB_OFFSET = 23;
    const int INDEX_TX2_MCS = 24;

    int rat = rildata->GetInt(INDEX_RAT);
    int band = rildata->GetInt(INDEX_BAND);
    int rbw = rildata->GetInt(INDEX_RBW);
    int scanMode = rildata->GetInt(INDEX_SCAN_MODE);
    int startFreq = rildata->GetInt(INDEX_START_FREQUENCY);
    int endFreq = rildata->GetInt(INDEX_END_FREQUENCY);
    int step = rildata->GetInt(INDEX_STEP);
    int antSel = rildata->GetInt(INDEX_ANTENNA_SELECTION);
    int sampling = rildata->GetInt(INDEX_SAMPLING_COUNT);
    int tx1 = rildata->GetInt(INDEX_TX1_ENABLED);
    int tx1Band = rildata->GetInt(INDEX_TX1_BAND);
    int tx1Bw = rildata->GetInt(INDEX_TX1_BW);
    int tx1Freq = rildata->GetInt(INDEX_TX1_FREQUENCY);
    int tx1Power = rildata->GetInt(INDEX_TX1_POWER);
    int tx1RbNum = rildata->GetInt(INDEX_TX1_RB_NUM);
    int tx1RbOffset = rildata->GetInt(INDEX_TX1_RB_OFFSET);
    int tx1Mcs = rildata->GetInt(INDEX_TX1_MCS);
    int tx2 = rildata->GetInt(INDEX_TX2_ENABLED);
    int tx2Band = rildata->GetInt(INDEX_TX2_BAND);
    int tx2Bw = rildata->GetInt(INDEX_TX2_BW);
    int tx2Freq = rildata->GetInt(INDEX_TX2_FREQUENCY);
    int tx2Power = rildata->GetInt(INDEX_TX2_POWER);
    int tx2RbNum = rildata->GetInt(INDEX_TX2_RB_NUM);
    int tx2RbOffset = rildata->GetInt(INDEX_TX2_RB_OFFSET);
    int tx2Mcs = rildata->GetInt(INDEX_TX2_MCS);

    RilLogV(" Scan Params {rat=%d band=%d rbw=%d scan mode=%d start freq=%d end freq=%d step=%d antenna sel=%d sampling=%d", rat, band, rbw, scanMode, startFreq, endFreq, step, antSel, sampling);
    RilLogV(" tx1{tx1=%d tx1 band=%d tx1 bw=%d tx1 freq=%d tx1 power=%d tx1 rbnum=%d rx1 rboffset=%d tx1 mcs=%d}", tx1, tx1Band, tx1Bw, tx1Freq, tx1Power, tx1RbNum, tx1RbOffset, tx1Mcs);
    RilLogV(" tx2{tx2=%d tx2 band=%d tx2 bw=%d tx2 freq=%d tx2 power=%d tx2 rbnum=%d rx2 rboffset=%d tx2 mcs=%d}}", tx2, tx2Band, tx2Bw, tx2Freq, tx2Power, tx2RbNum, tx2RbOffset, tx2Mcs);

    ProtocolSensorBuilder builder;
    ModemData *pModemData = builder.BuildScanRssi(rat, band, rbw, scanMode, startFreq, endFreq, step, antSel, sampling,
                                                tx1, tx1Band, tx1Bw, tx1Freq, tx1Power, tx1RbNum, tx1RbOffset, tx1Mcs,
                                                tx2, tx2Band, tx2Bw, tx2Freq, tx2Power, tx2RbNum, tx2RbOffset, tx2Mcs);
    if (SendRequest(pModemData, TIMEOUT_OEM_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int RssiScanHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }

    return 0;
}

/**
 * RssiScanResultHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RssiScanResultHandler, MSG_MISC_SCAN_RSSI_RESULT_RECEIVED)

int RssiScanResultHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscRssiScanResultAdapter adapter(pModemData);
    int total = adapter.GetTotalPage();
    int current = adapter.GetCurrentPage();
    int startFreq = adapter.GetStartFrequency();
    int endFreq = adapter.GetEndFrequency();
    int step = adapter.GetStep();
    int resultSize = adapter.GetScanResultSize();
    INT16 *result = adapter.GetScanResult();
    RilLogV(" Scan Result {(%d/%d) Freq[%d,%d] step=%d resultSize=%d",
                current, total, startFreq, endFreq, step, resultSize);

    MiscDataBuilder builder;
    const RilData *rildata = builder.BuildRssiScanResult(total, current, startFreq, endFreq, step, result, resultSize);
    if (rildata != NULL){
        OnUnsolicitedResponse(RIL_UNSOL_OEM_SCAN_RSSI_RESULT, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    return 0;
}
