/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include "deviceinfo.h"
#include "miscservice.h"
#include "miscdata.h"
#include "miscdatabuilder.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"
#include "service.h"
#include <librilutils/systemproperty.h>

#define TIMEOUT_DEVICE_INFO_DEFAULT                     5000
#define TIMEOUT_BASEBAND_VERSION                 5000
#define TIMEOUT_IMEI                             5000
#define TIMEOUT_DEVICE_IDENTITY                  5000
#define TIMEOUT_SET_OPEN_CARRIER                 180000
#define TIMEOUT_SET_CARRIER_INFO_IMSI_ENCRYPT    300000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * BasebandVersionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(BasebandVersionHandler, MSG_MISC_BASEBAND_VER)

int BasebandVersionHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.GetBaseBandVersion();
    if (SendRequest(pModemData, TIMEOUT_BASEBAND_VERSION, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int BasebandVersionHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscVersionAdapter adapter(pModemData);
    MiscDataBuilder builder;
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        const char *swver = adapter.GetSwVer();
        const char *hwver = adapter.GetHwVer();
        SystemProperty::Set(RIL_VENDOR_BASEBAND_HW_VERSION, hwver);

        const RilData *rildata = builder.BuildBaseBandVersionResponse(swver);
        if (rildata != NULL){
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * ImeiHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ImeiHandler, MSG_MISC_GET_IMEI)

int ImeiHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.GetIMEI();
    if (SendRequest(pModemData, TIMEOUT_IMEI, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int ImeiHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscIMEIAdapter adapter(pModemData);
    int len = adapter.GetIMEILen();
    const BYTE *imei = adapter.GetIMEI();
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildIMEIResponse(len, imei);
        if (rildata != NULL){
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * ImeisvHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ImeisvHandler, MSG_MISC_GET_IMEISV)

int ImeisvHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.GetIMEISV();
    if (SendRequest(pModemData, TIMEOUT_IMEI, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int ImeisvHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscIMEISVAdapter adapter(pModemData);
    int len = adapter.GetIMEISVLen();
    static BYTE send_imeisv[MAX_IMEISV_LEN];
    const BYTE *imeisv = adapter.GetIMEISV();

    if (imeisv == NULL) {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
        return 0;
    }
#define SV_START_BIT_RECEIVE  (15)
#define SV_START_BIT_RESPONSE (14)
#define SV_FIELD_LEN          (2)

    /* remove the checksum field (1byte)
        - IMEI format : AA-BBBBBB-CCCCCC-D : D is checksum
        - IMEISV format : AA-BBBBBB-CCCCCC-EE : EE is software version*/
    memset(send_imeisv, 0, MAX_IMEISV_LEN);
    memcpy(send_imeisv, imeisv, MAX_IMEISV_LEN);
    memmove(&send_imeisv[SV_START_BIT_RESPONSE], &send_imeisv[SV_START_BIT_RECEIVE], SV_FIELD_LEN);
    send_imeisv[MAX_IMEISV_LEN-1] = 0;

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildIMEISVResponse(len, (const BYTE*)send_imeisv);
        if (rildata != NULL){
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * DeviceIdentityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DeviceIdentityHandler, MSG_MISC_DEV_IDENTITY)

int DeviceIdentityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.GetDevID();
    if (SendRequest(pModemData, TIMEOUT_DEVICE_IDENTITY, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int DeviceIdentityHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscDeviceIDAdapter adapter(pModemData);
    int imeilen = adapter.GetIMEILen();
    const BYTE *imei = adapter.GetIMEI();
    int imeisvlen = adapter.GetIMEISVLen();
    const BYTE *imeisv = adapter.GetIMEISV();
    int meidlen = adapter.GetMEIDLen();
    const BYTE *meid = adapter.GetMEID();
    int esnlen = adapter.GetESNLen();
    const BYTE *esn = adapter.GetESN();
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildDevIDResponse(imeilen, imei, imeisvlen, imeisv, meidlen, meid, esnlen, esn);
        if (rildata != NULL){
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * ScreenStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ScreenStateHandler, MSG_MISC_SCREEN)

int ScreenStateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int scrState = rildata->GetInt() & 0xff;    // get lower byte
    RilLogV("scrState request rildata->GetInt() is = %d", rildata->GetInt());
    RilLogV("scrState request is = %d", scrState);
    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.SetScreenState(scrState);
    if (SendRequest(pModemData, TIMEOUT_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    RilLogV("%s() Screen %s", __FUNCTION__, scrState ? "ON" : "OFF");

    return 0;
}

int ScreenStateHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * DeviceStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DeviceStateHandler, MSG_MISC_SEND_DEVICE_STATE)

int DeviceStateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int deviceStateType = rildata->GetInt(0);
    int deviceState = rildata->GetInt(1);
    RilLog("deviceStateType=%d deviceState=%d", deviceStateType, deviceState);

    // TODO: do action according to device state type
    // enum {RIL_DST_POWER_SAVE_MODE, RIL_DST_CHARGING_STATE, RIL_DST_LOW_DATA_EXPECTED}
    // modem actions have not been defined yet.

    OnRequestComplete(RIL_E_SUCCESS);

    return 0;
}

int DeviceStateHandler::OnResponse(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}

/**
 * PinControlHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PinControlHandler, MSG_MISC_OEM_SET_PIN_CONTROL)

int PinControlHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == nullptr) {
        RilLogE("rildata is NULL");
        return -1;
    }

    const char *miscData = (char *) rildata->GetRawData();
    const int minPinControlDataLen = sizeof(BYTE) * 2;
    if (miscData == nullptr || rildata->GetSize() < minPinControlDataLen) {
        return -1;
    }
    BYTE signal = miscData[0];
    BYTE status = miscData[1];

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.SetPinControl(signal, status);
    if (SendRequest(pModemData, TIMEOUT_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int PinControlHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}


/**
 * UnsolPinControlHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolPinControlHandler, MSG_MISC_UNSOL_OEM_PIN_CONTROL)

int UnsolPinControlHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscPinControlAdapter adapter(pModemData);
    BYTE result[2];
    result[0] = adapter.GetSignal();
    result[1] = adapter.GetStatus();

    OnUnsolicitedResponse(RIL_UNSOL_OEM_PIN_CONTROL, result, 2);

    return 0;
}

/**
 * SGCHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SGCHandler, MSG_MISC_OEM_SEND_SGC)

int SGCHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    const int TargetOp = rildata->GetInt();
    RilLogV("[%s] %s(), SGCVal : %d", mServiceName, __FUNCTION__, TargetOp);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.SendSGCValue(TargetOp, 0/*reserved1*/, 0/*reserved2*/);
    if (SendRequest(pModemData, 2000, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SGCHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * DeviceInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DeviceInfoHandler, MSG_MISC_SET_DEVICE_INFO)

int DeviceInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    StringsRequestData *rildata = (StringsRequestData *)msg->GetRequestData();
    const char* pModel = rildata->GetString(0);
    const char* pSwVer = rildata->GetString(1);
    const char* pProductName = rildata->GetString(2);
    const char* pApOsVersion = rildata->GetString(3);

    RilLogV("[%s] %s(), Model : %s, SwVer : %s, Product Name : %s OS Ver : %s", mServiceName, __FUNCTION__, pModel, pSwVer, pProductName, pApOsVersion);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.SendDeviceInfo(pModel, pSwVer, pProductName, pApOsVersion);
    if (SendRequest(pModemData, TIMEOUT_MISC_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int DeviceInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s()", mServiceName, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscSetDeviceInfoAdapter adapter(pModemData);
    if (mPhoneId == RIL_SOCKET_1) {
        RilProperty *prop = mService->GetRilApplicationProperty();
        string key1 = STRING_CELL_INFO;
        prop->Put(key1, adapter.GetVersionInfo(key1.c_str()));
        string key2 = STRING_CELL_IDENTITY;
        prop->Put(key2, adapter.GetVersionInfo(key2.c_str()));
        string key3 = STRING_SIGNAL_STRENGTH;
        prop->Put(key3, adapter.GetVersionInfo(key3.c_str()));
    }

    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * OpenCarrierInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OpenCarrierInfoHandler, MSG_MISC_SET_OPEN_CARRIER_INFO)

int OpenCarrierInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    StringsRequestData* pReq = (StringsRequestData*)msg->GetRequestData();
    UINT32 openCarrierIndex = strtol(pReq->GetString(0), NULL, 10);
    const char *plmn = pReq->GetString(1);
    RilLogI("[%s::%s] OCI:0x%x PLMN:%s", mServiceName, __FUNCTION__, openCarrierIndex, plmn);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetOpenCarierInfo(openCarrierIndex, plmn);
    if (SendRequest(pModemData, TIMEOUT_SET_OPEN_CARRIER, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    BYTE ocInfo[MAX_PLMN_LEN + sizeof(UINT32)];
    memset(ocInfo, 0, sizeof(ocInfo));
    memcpy(ocInfo, plmn, strlen(plmn));
    if(strlen(plmn) == 5) ocInfo[MAX_PLMN_LEN - 1] = '#';
    memcpy(ocInfo + MAX_PLMN_LEN, &openCarrierIndex, sizeof(UINT32));
    OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_OPEN_CARRIER_INFO, ocInfo, sizeof(ocInfo));

    RilLogI("[%s] %s() [-->", mServiceName,__FUNCTION__);
    return 0;
}

int OpenCarrierInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    /* This RIL req is triggered internally in vendor RIL, send sucess always */
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

/**
 * HwConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(HwConfigHandler, MSG_MISC_GET_HARDWARE_CONFIG)

int HwConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    MiscService *misc = MiscService::AsService(mService);
    if (misc == NULL) {
        return -1;
    }

    int subscriptionSource = misc->GetCdmaSubscriptionSource();
    int radioState = mRilContext->GetCurrentRadioState();
    int hwConfigState = (radioState == RADIO_STATE_ON)? RIL_HARDWARE_CONFIG_STATE_STANDBY: RIL_HARDWARE_CONFIG_STATE_DISABLED;

    UINT uRat = 0;
    uRat = RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B;

    MiscDataBuilder builder;
    const RilData *pRilData = NULL;
    if(subscriptionSource == CDMA_SUBSCRIPTION_SOURCE_NV) {
        pRilData = builder.BuildHardwareConfigNV(NULL, hwConfigState, 0, uRat, 1, 1, RIL_SOCKET_NUM);
    }
    else {
        pRilData = builder.BuildHardwareConfigRuim(NULL, hwConfigState, NULL);
    }

    if (pRilData == NULL ) return OnRequestComplete(RIL_E_GENERIC_FAILURE);
    OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());

    if (pRilData != NULL) {
        delete pRilData;
    }

    return 0;
}

int HwConfigHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnRequest(msg);
}

/**
 * HwConfigChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(HwConfigChangedHandler, MSG_MISC_IND_HARDWARE_CONFIG_CHANGED)

int HwConfigChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscHwConfigChangeAdapter adapter(pModemData);
    RIL_HardwareConfig *pRsp = NULL;
    int num = adapter.GetNum();
    if ( num > 0 ) {
        pRsp = (RIL_HardwareConfig *)calloc(num, sizeof(RIL_HardwareConfig));
        adapter.GetData(pRsp, num);
    } else {
        num = 0;
    }

    if (pRsp != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_HARDWARE_CONFIG_CHANGED, pRsp, num * sizeof(RIL_HardwareConfig));
        free(pRsp);
    }

    return 0;
}

/**
 * ModemRestartHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ModemRestartHandler, MSG_MISC_IND_MODEM_RESTART)

int ModemRestartHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolIndAdapter adapter(pModemData);
    unsigned int len = adapter.GetParameterLength();
    const char *pData = adapter.GetParameter();
    OnUnsolicitedResponse(RIL_UNSOL_MODEM_RESTART, pData, len);

    return 0;
}

/**
 * SetCarrierInfoImsiEncryptionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCarrierInfoImsiEncryptionHandler, MSG_MISC_SET_CARRIER_INFO_IMSI_ENCRYPTION)

int SetCarrierInfoImsiEncryptionHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    CarrierInfoForImsiEncryptionData *rildata = (CarrierInfoForImsiEncryptionData *)msg->GetRequestData();
    if (rildata == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    int nResult = -1;
    char *pMcc = rildata->GetMcc();
    char *pMnc = rildata->GetMnc();
    int keyLen = rildata->GetCarrierKeyLen();
    BYTE *pKey = rildata->GetCarrierKey();
    int keyIdLen = rildata->GetKeyIdLen();
    char *pKeyId = rildata->GetKeyIdentifier();
    LONG expTime = rildata->GetEpirationTime();
    int keyType = rildata->GetKeyType(); // UNKNOWN : 0, EPDG : 1, WLAN :2

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetCarrierInfoImsiEncryption(pMcc, pMnc, keyLen, pKey, keyIdLen, pKeyId, expTime, keyType);
    nResult = SendRequest(pModemData, TIMEOUT_SET_CARRIER_INFO_IMSI_ENCRYPT, mMessageId);

    return (nResult<0)? -1: 0;
}

int SetCarrierInfoImsiEncryptionHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    UINT errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
    OnRequestComplete(errorCode, NULL, 0);
    return 0;
}

/**
 * UnsolCarrierInfoImsiEncryptionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCarrierInfoImsiEncryptionHandler, MSG_MISC_IND_CARRIER_INFO_IMSI_ENCRYPTION)

int UnsolCarrierInfoImsiEncryptionHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    OnUnsolicitedResponse(RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION);
    return 0;
}

/**
 * SetIndicationFilterHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetIndicationFilterHandler, MSG_MISC_SET_INDICATION_FILTER)

int SetIndicationFilterHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    // RIL_REQUEST_SCREEN_STATE - DEPRECATED
    // use RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER to turn on/off unsolicited

    // indicationFilter : a 32-bit bitmask of RIL_IndicationFilter
    // RIL_INDFILTER_SIGNAL_STRENGTH/RIL_INDFILTER_FULL_NETWORK_STATE/RIL_INDFILTER_DATA_CALL_DORMANCY_CHANGED
    // RIL_INDFILTER_LINK_CAPACITY_ESTIMATE/RIL_INDFILTER_PHYSICAL_CHANNEL_CONFIG
    // RIL_INDFILTER_REGISTRATION_FAILURE/RIL_INDFILTER_BARRING_INFO
    int indicationFilter = rildata->GetInt() & 0xFFFFFFFF;
    RilLogI("IndicationFilter : 0x%x", indicationFilter);

    // internal request (legacy)
    int screenState = (indicationFilter & RIL_INDFILTER_SIGNAL_STRENGTH) ? 1 : 0;
    mRilContext->OnRequest(RIL_REQUEST_SCREEN_STATE, &screenState , sizeof(int), NULL);

    // set indication filter
    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetUnsolicitedResponseFilter(indicationFilter);
    if (SendRequest(pModemData, TIMEOUT_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetIndicationFilterHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        RilLogW("errorCode: %d", errorCode);
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

/**
 * SignalStrengthReportingCriteriaHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SignalStrengthReportingCriteriaHandler, MSG_MISC_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA)

int SignalStrengthReportingCriteriaHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    SignalStrengthReportingCriteria *rildata = (SignalStrengthReportingCriteria *)msg->GetRequestData();
    if (rildata == NULL || !rildata->isValidArguement())  {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    int hysteresisMs = rildata->GetHysteresisMs();
    int hysteresisDb = rildata->GetHysteresisDb();
    int numOfThresholdsDbm = rildata->GetNumOfThresholdsDbm();
    int *thresholdsDbm = rildata->GetThresholdsDbm();
    int accessNetwork = rildata->GetAccessNetwork();
    int measureType = rildata->GetMeasureType();
    bool isEnabled = rildata->GetIsEnabled();
    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetSignalReportCriteria(hysteresisMs, hysteresisDb,
                                    numOfThresholdsDbm, thresholdsDbm, accessNetwork, measureType, isEnabled);
    if (SendRequest(pModemData, TIMEOUT_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SignalStrengthReportingCriteriaHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        RilLogW("errorCode: %d", errorCode);
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

/**
 * OemSignalStrengthReportingCriteriaHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OemSignalStrengthReportingCriteriaHandler, MSG_MISC_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA)

int OemSignalStrengthReportingCriteriaHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_MSG(msg);

    SignalStrengthReportingCriteria_V2_0 *rildata = (SignalStrengthReportingCriteria_V2_0 *)msg->GetRequestData();
    if (rildata == NULL || !rildata->isValidArguements()) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    int total = rildata->GetSignalThresholdInfoNum();
    for (int i = 0; i < total; i++) {
        auto& info = rildata->at(i);
        RIL_SignalStrengthReportingCriteria_V1_5 ssc {};
        ssc.accessNetwork = info.ran;
        ssc.signalThresholdInfo.signalMeasurement = (RIL_SignalMeasurementType) info.signalMeasurement;
        ssc.signalThresholdInfo.hysteresisMs = info.hysteresisMs;
        ssc.signalThresholdInfo.hysteresisDb = info.hysteresisDb;
        if (info.thresholds.size() > 0) {
            ssc.signalThresholdInfo.len_thresholds = info.thresholds.size();
            ssc.signalThresholdInfo.thresholds = (int *)(info.thresholds.data());
        }
        ssc.signalThresholdInfo.isEnabled = info.isEnabled;
        int request = ENCODE_REQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5));
        mService->OnRequest(request, &ssc, sizeof(RIL_SignalStrengthReportingCriteria_V1_5));
    }

    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int OemSignalStrengthReportingCriteriaHandler::OnResponse(Message */* msg */) {
    RilLogW("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}

/**
 * LinkCapacityReportingCriteriaHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(LinkCapacityReportingCriteriaHandler, MSG_MISC_SET_LINK_CAPACITY_REPORTING_CRITERIA)

int LinkCapacityReportingCriteriaHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    LinkCapacityReportingCriteria *rildata = (LinkCapacityReportingCriteria *)msg->GetRequestData();
    if (rildata == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    int hysteresisMs = rildata->GetHysteresisMs();
    int hysteresisDlKbps = rildata->GetHysteresisDlKbps();
    int hysteresisUlKbps = rildata->GetHysteresisUlKpbs();
    int numOfThresholdsDownlinkKbps = rildata->GetNumOfThresholdsDownlinkKbps();
    int* thresholdsDownlinkKbps = rildata->GetThresholdsDownlinkKbps();
    int numOfThresholdsUplinkKbps = rildata->GetNumOfThresholdsUplinkKbps();
    int* thresholdsUplinkKbps = rildata->GetThresholdsUplinkKbps();
    int accessNetwork = rildata->GetAccessNetwork();

    // test invalidHysteresisDlKbps/invalidHysteresisUlKbps for VTS 1.2/1.5
    // hysteresisDlKbps/hysteresisUlKbps must be smaller than the smallest threshold delta
    // ASSERT_TRUE(MIN(thresholdsDownlinkKbps[i+1] - thresholdsDownlinkKbps[i]) >= hysteresisDlKbps)
    // ASSERT_TRUE(MIN(thresholdsUplinkKbps[i+1] - thresholdsUplinkKbps[i]) >= hysteresisUlKbps)
    // example
    // vector {1000, 5000, 10000, 20000}  hysteresisDlKbps 5000
    //   : invalid argument. delta is 4000
    if (numOfThresholdsDownlinkKbps > 0 && thresholdsDownlinkKbps != NULL) {
        for (int i = 0; i < numOfThresholdsDownlinkKbps - 1; i++) {
            if (hysteresisDlKbps < 0 ||
                    thresholdsDownlinkKbps[i] > thresholdsDownlinkKbps[i+1] ||
                    thresholdsDownlinkKbps[i+1] - thresholdsDownlinkKbps[i] < hysteresisDlKbps) {
                OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
                return 0;
            }
        } // end for i ~
    }

    if (numOfThresholdsUplinkKbps > 0 && thresholdsUplinkKbps != NULL) {
        for (int i = 0; i < numOfThresholdsUplinkKbps - 1; i++) {
            if (hysteresisUlKbps < 0 ||
                    thresholdsUplinkKbps[i] > thresholdsUplinkKbps[i+1] ||
                    thresholdsUplinkKbps[i+1] - thresholdsUplinkKbps[i] < hysteresisUlKbps) {
                OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
                return 0;
            }
        } // end for i ~
    }

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetLinkCapaReportCriteria(
            hysteresisMs, hysteresisDlKbps, hysteresisUlKbps, numOfThresholdsDownlinkKbps,
            thresholdsDownlinkKbps, numOfThresholdsUplinkKbps, thresholdsUplinkKbps, accessNetwork);
    if (SendRequest(pModemData, TIMEOUT_DEVICE_INFO_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int LinkCapacityReportingCriteriaHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }
    return 0;
}

/**
 * SetFeatureInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetFeatureInfoHandler, MSG_MISC_SET_FEATURE_INFO)

int SetFeatureInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    bool iwlanApAssit = mService->GetRilApplicationProperty()->GetBool(RIL_APP_IWLAN_IN_AP_ASSIT_MODE);
    RilLogV("===Feature list ===");
    RilLogV("  iwlanApAssist: %d", iwlanApAssit);
    RilLogV("=== Feature end ===");

    RilProperty bundle;
    bundle.Put("iwlan_ap_assist", iwlanApAssit);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildSetFeatureInfo(bundle);
    if (SendRequest(pModemData, TIMEOUT_MISC_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetFeatureInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * RfConnectionStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RfConnectionStateHandler, MSG_MISC_RF_CONNECTION_STATE_IND)

int RfConnectionStateHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if (mPhoneId != RIL_SOCKET_1) {
        return 0;
    }

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRfConnectionStateAdapter adapter(pModemData);
    BYTE state = adapter.GetState();

    RilLogV("[%d] RF connection: %d -> %d", mPhoneId,
            SystemProperty::GetInt(RIL_VENDOR_RF_CONNECTION, 0), state);

    if (state > 0)
        SystemProperty::Set(RIL_VENDOR_RF_CONNECTION, "1");
    else
        SystemProperty::Set(RIL_VENDOR_RF_CONNECTION, "0");

    OnUnsolicitedResponse(RIL_UNSOL_OEM_SAR_RF_CONNECTION, &state, sizeof(state));
    return 0;
}
