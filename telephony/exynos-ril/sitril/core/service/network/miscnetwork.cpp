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
 * miscnetwork.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */
#include "miscnetwork.h"
#include "miscdatabuilder.h"
#include "netdata.h"
#include "netdatabuilder.h"
#include "networkservice.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "commondef.h"

#define TIMEOUT_MISC_NETWORK_DEFAULT        5000
#define TIMEOUT_PREFERRED_CALL_CAPABILITY   5000
#define TIMEOUT_VOICE_OPERATION             5000

#define NET_SIM_EF_PNN    (0x6fc5)
#define NET_SIM_EF_OPL    (0x6fc6)
#define NET_SIM_EF_OPL5G  (0x4f08)

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

namespace radio_network = android::hardware::radio::impl::network;
namespace V2_0 = android::hardware::radio::impl::modem::V2_0;

/**
 * CancelAvailableNetworkstHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CancelAvailableNetworkstHandler, MSG_MISC_OEM_CANCEL_AVAILABLE_NETWORKS)

int CancelAvailableNetworkstHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    // Cancel requested PLMN searching
    ProtocolNetworkScanBuilder builder;
    ModemData *pModemData = builder.BuildCancelQueryAvailableNetwork();
    if (SendRequest(pModemData, 5000, mMessageId) < 0) {
        RilLogW("Fail to send Cancel query available networks request ");
        return -1;
    }
    return 0;
}

int CancelAvailableNetworkstHandler::OnResponse(Message *msg)
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
 * SetPreferredCallCapabilityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPreferredCallCapabilityHandler, MSG_MISC_OEM_SET_PREFERRED_CALL_CAPABILITY)

int SetPreferredCallCapabilityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    const char *MiscData = (char *) rildata->GetRawData();
    BYTE mode = MiscData[0];
    RilLogI("[%s] Preferred Call Capability mode = %d", mServiceName, mode);

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetPreferredCallCapability(mode);
    if (SendRequest(pModemData, TIMEOUT_PREFERRED_CALL_CAPABILITY, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetPreferredCallCapabilityHandler::OnResponse(Message *msg)
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
 * GetPreferredCallCapabilityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPreferredCallCapabilityHandler, MSG_MISC_OEM_GET_PREFERRED_CALL_CAPABILITY)

int GetPreferredCallCapabilityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetPreferredCallCapability();
    if (SendRequest(pModemData, TIMEOUT_PREFERRED_CALL_CAPABILITY, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetPreferredCallCapabilityHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetPreferredCallCapability adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int mode = adapter.GetMode();
        RilLogV("preferred call capability mode=%d", mode);
        OnRequestComplete(errorCode, &mode, sizeof(mode));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetVoiceOperationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetVoiceOperationHandler, MSG_MISC_SET_VOICE_OPERATION)

int SetVoiceOperationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)(msg->GetRequestData());
    int mode = rildata->GetInt();
    RilLogI("[%s] Voice Operation mode = %d", mServiceName, mode);

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetVoiceOperation(mode);
    if (SendRequest(pModemData, TIMEOUT_VOICE_OPERATION, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetVoiceOperationHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if ( errorCode == RIL_E_SUCCESS ) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }

    return 0;
}

/**
 * GetActivityInfotHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetActivityInfotHandler, MSG_MISC_GET_ACTIVITY_INFO)

int GetActivityInfotHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.GetModemActivityInfo();
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetActivityInfotHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolMiscGetActivityInfoRatSpecificAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        const V2_0::RIL_ActivityStatsInfo& info = adapter.info();

        int halVer = mService->GetCurrentRequestHalVersionCode();
        auto rildata = std::unique_ptr<const RilData>(ActivityStatsInfoBuilder(halVer).build(info));
        if (rildata) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * LceStartHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(LceStartHandler, MSG_MISC_LCE_START)

int LceStartHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int interval = rildata->GetInt(0);
    int mode = rildata->GetInt(1);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildLceStart(mode, interval);
    // Currently Not supported
    if(pModemData == NULL) {
        OnRequestComplete(RIL_E_LCE_NOT_SUPPORTED);
        return 0;
    }
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0 ) {
        return -1;
    }

    return 0;
}

int LceStartHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscLceAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    RilLogE("[%s] %s() errorCode=%d", mServiceName, __FUNCTION__, errorCode);
    if (errorCode == RIL_E_SUCCESS) {
        RIL_LceStatusInfo lceStatusInfo;
        lceStatusInfo.lce_status = adapter.getLceStatus();
        lceStatusInfo.actual_interval_ms = adapter.getActualIntervalMs();
        OnRequestComplete(RIL_E_SUCCESS, &lceStatusInfo, sizeof(lceStatusInfo));
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * LceStopHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(LceStopHandler, MSG_MISC_LCE_STOP)

int LceStopHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildLceStop();
    // Currently Not supported
    if(pModemData == NULL) {
        OnRequestComplete(RIL_E_LCE_NOT_SUPPORTED);
        return 0;
    }
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0 )
    {
        return -1;
    }

    return 0;
}

int LceStopHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscLceAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RIL_LceStatusInfo lceStatusInfo;
        lceStatusInfo.lce_status = adapter.getLceStatus();
        lceStatusInfo.actual_interval_ms = adapter.getActualIntervalMs();
        OnRequestComplete(RIL_E_SUCCESS, &lceStatusInfo, sizeof(lceStatusInfo));
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * LcePullDataHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(LcePullDataHandler, MSG_MISC_PULL_LCEDATA)

int LcePullDataHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.BuildLcePullLceData();
    // Currently Not supported but should return NONE or INTERNAL_ERR
    if(pModemData == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }
    if ( SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0 )
    {
        return -1;
    }

    return 0;
}

int LcePullDataHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscLceAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RIL_LceDataInfo lceDataInfo;
        lceDataInfo.last_hop_capacity_kbps = adapter.getDlCapacityKbps();
        lceDataInfo.confidence_level = adapter.getConfidencelevel();
        lceDataInfo.lce_suspended = adapter.getLceSuspended();
        OnRequestComplete(RIL_E_SUCCESS, &lceDataInfo, sizeof(lceDataInfo));
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * LceDataRecvHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(LceDataRecvHandler, MSG_MISC_IND_CURRENT_LINK_CAPACITY_ESTIMATE)

int LceDataRecvHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscCurrentLinkCapacityEstimate adapter(pModemData);
    int halVer = RilApplication::RIL_HalVersionCode;
    CurrentLinkCapacityEstimateBuilder buidler(halVer);
    const RilData *rildata = buidler.Build(adapter.GetDlCapaKbps(), adapter.GetUlCapaKbps(), adapter.GetSecDlCapaKbps(), adapter.GetSecUlCapaKbps());
    if (rildata != NULL){
        OnUnsolicitedResponse(RIL_UNSOL_LCEDATA_RECV, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    return 0;
}

/**
 * GetNeighboringCellIdsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetNeighboringCellIdsHandler, MSG_MISC_GET_NEIGHBORING_CELL_IDS)

int GetNeighboringCellIdsHandler::OnRequest(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    // To do
    // For VTS test, return success
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int GetNeighboringCellIdsHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnResponse(msg);
}

/**
 * SetLocationUpdatesHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetLocationUpdatesHandler, MSG_MISC_SET_LOCATION_UPDATES)

int SetLocationUpdatesHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable = rildata->GetInt();
    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetLocationUpdates(enable);
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetLocationUpdatesHandler::OnResponse(Message *msg)
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
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}

/**
 * SetSuppSvcNotificationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSuppSvcNotificationHandler, MSG_MISC_SET_SUPP_SVC_NOTIFICATION)

int SetSuppSvcNotificationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable = rildata->GetInt() & 0xFFFFFFFF;
    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetSuppSvcNotification(enable);
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        OnRequestComplete(RIL_E_SUCCESS);
        return 0;
    }

    return 0;
}

int SetSuppSvcNotificationHandler::OnResponse(Message *msg)
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
 * EnableModemHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EnableModemHandler, MSG_MISC_ENABLE_MODEM)

int EnableModemHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    int nResult = -1;
    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if(rildata)
    {
        /*
        * data is int *
        * ((int *)data)[0] is > 0 to turn on the logical modem
        * ((int *)data)[0] is == 0 to turn off the logical modem
        */
        int mode = SIT_PWR_STATCK_DISABLE;
        if(rildata->GetInt()>0)
        {
            mode = SIT_PWR_STATCK_ENABLE;
        }

        ProtocolMiscNetworkBuilder builder;
        ModemData *pModemData = builder.BuildSetStatckStatus(mode);
        nResult = SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId);
    }

    return (nResult<0)? -1: 0;
}

int EnableModemHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) errorCode = RIL_E_MODEM_ERR;
    OnRequestComplete(errorCode, NULL, 0);
    return 0;
}

/**
 * GetModemStackStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetModemStackStatusHandler, MSG_MISC_GET_MODEM_STACK_STATUS)

int GetModemStackStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    int nResult = -1;
    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetStatckStatus();
    nResult = SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId);
    return (nResult<0)? -1: 0;
}

int GetModemStackStatusHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int status = 1;
    if (errorCode == RIL_E_SUCCESS)
    {
        ProtocolMiscGetStackStatusAdapter adapter(pModemData);
        status = adapter.GetMode();
    }
    else
    {
        errorCode = RIL_E_MODEM_ERR;
    }

    MiscDataBuilder builder;
    const RilData *pRilData = builder.BuildGetModemStatus(status);
    if ( pRilData == NULL )
    {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    else
    {
        OnRequestComplete(errorCode, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
    }
    return 0;
}

/**
 * NitzTimeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NitzTimeHandler, MSG_MISC_UNSOL_NITZ_TIME_RECEIVED)

int NitzTimeHandler::OnIndication(Message *msg)
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

    ProtocolMiscNITZTimeAdapter adapter(pModemData);
    int year = adapter.Year();
    int month = adapter.Month();
    int day = adapter.Day();
    int hour = adapter.Hour();
    int minute = adapter.Minute();
    int second = adapter.Second();
    int timezone = adapter.TimeZone();
    int daylightadjust = adapter.DayLightAdjust();
    int64_t age = adapter.age();
    int64_t timestamp = msg->timestamp();

    // TODO RilApplication::RIL_HalVersionCode might change to IRadioNetwork version code
    int halVer = RilApplication::RIL_HalVersionCode;
    auto rildata = std::unique_ptr<const RilData>(NitzTimeBuilder(halVer).
                                        BuildNitzTime(year, month, day, hour, minute,
                                                    second, timezone, daylightadjust,
                                                    age, timestamp));
    if (rildata) {
        OnUnsolicitedResponse(RIL_UNSOL_NITZ_TIME_RECEIVED, rildata->GetData(), rildata->GetDataLength());
    }
    return 0;
}

/**
 * GetNitzTimeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetNitzTimeHandler, MSG_MISC_GET_NITZ_TIME)
int GetNitzTimeHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetNitzTime();
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetNitzTimeHandler::OnResponse(Message *msg) {
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

    ProtocolMiscGetNITZTimeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int year = adapter.Year();
        int month = adapter.Month();
        int day = adapter.Day();
        int hour = adapter.Hour();
        int minute = adapter.Minute();
        int second = adapter.Second();
        int timezone = adapter.TimeZone();
        int daylightadjust = adapter.DayLightAdjust();
        int64_t age = adapter.age();
        int64_t timestamp = msg->timestamp();

        auto halVer = mService->GetCurrentRequestHalVersionCode();
        auto rildata = std::unique_ptr<const RilData>(NitzTimeBuilder(halVer).
                                            BuildNitzTime(year, month, day, hour, minute,
                                                        second, timezone, daylightadjust,
                                                        age, timestamp));
        if (rildata) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * ImsSupportServiceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ImsSupportServiceHandler, MSG_NET_OEM_GET_IMS_SUPPORT_SERVICE)

int ImsSupportServiceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkRegistrationBuilder builder;
    ModemData *pModemData = builder.BuildNetworkRegistrationState(DOMAIN_DATA_NETWORK);
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int ImsSupportServiceHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetDataRegStateAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result[5];
        result[0] = (adapter.GetImsVops() ? 1 : 0);
        result[1] = (adapter.GetEmcService() ? 1 : 0);
        result[2] = mService->GetRilSocketId();
        result[3] = adapter.GetRadioTech();
        result[4] = adapter.GetRegState();
        OnRequestComplete(RIL_E_SUCCESS, result, sizeof(result));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * UnsolSimFileInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolSimFileInfoHandler, MSG_NET_IND_SIM_FILE_INFO)

void UnsolSimFileInfoHandler::OnCreate() {
    if (mService) {
        mService->addSimStatusListener(this);
    }
}

void UnsolSimFileInfoHandler::OnDestroy() {
    if (mService) {
        mService->removeSimStatusListener(this);
    }
}

void UnsolSimFileInfoHandler::OnSimStatusChanged(int cardState, int /* appState */) {
    if ( cardState == RIL_CARDSTATE_ABSENT ) {
        // reset PNN/OPL for SIM
        RilLog("[%d] Reset PNN/OPL by SIM removal.", mPhoneId);
        EonsResolver::createForPhoneId(mPhoneId).reset();
    }
}

int UnsolSimFileInfoHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetSimFileInfoAdapter adapter(pModemData);
    int simFileId = adapter.GetSimFileId();
    int recordLen = adapter.GetRecordLen();
    int numOfRecord = adapter.GetNumOfRecords();
    BYTE **simFileData = adapter.GetSimFileData();
    RilLogI("[%s] %s(): EF:0x%x, recLen = %d, numOfRec = %d", mServiceName, __FUNCTION__,
            simFileId, recordLen, numOfRecord);
    EonsResolver &resolver = EonsResolver::createForPhoneId(mPhoneId);
    if (simFileId == NET_SIM_EF_PNN) {
        resolver.resetPnnData();
        if (numOfRecord > 0) resolver.setPnnData(simFileData, recordLen, numOfRecord);
    } else if (simFileId == NET_SIM_EF_OPL) {
        resolver.resetOplData();
        if (numOfRecord > 0) resolver.setOplData(simFileData, recordLen, numOfRecord);
    } else if (simFileId == NET_SIM_EF_OPL5G) {
        resolver.resetOpl5GData();
        if (numOfRecord > 0) resolver.setOpl5GData(simFileData, recordLen, numOfRecord);
    } else {
    }

    return 0;
}

/**
 * SetDataThrottlingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDataThrottlingHandler, MSG_MISC_SET_DATA_THROTTLING)

int SetDataThrottlingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    DataThrottlingReqData *rildata = (DataThrottlingReqData *)msg->GetRequestData();
    int dataThrottlingAction = rildata->GetDataThrottlingAction();
    int64_t completionDurationMillis = rildata->GetCompletionDurationMillis();
    RilLogI("[%s] dataThrottlingAction = %d completionWindowsecs = %d", mServiceName, dataThrottlingAction, completionDurationMillis);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetDataThrottling(dataThrottlingAction, completionDurationMillis);
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetDataThrottlingHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    // To pass a VTS/CTS for setDataThrottling
    errorCode = RIL_E_SUCCESS;

    OnRequestComplete(errorCode);

    return 0;
}

/**
 * GetSystemSelectionChannelsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetSystemSelectionChannelsHandler, MSG_NET_GET_SYSTEM_SELECTION_CHANNELS)
int GetSystemSelectionChannelsHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetSystemSelectionChannels();

    if (pModemData == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0)
    {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetSystemSelectionChannelsHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetSystemSelectionChannelsAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int specifiersLength = adapter.GetSpecifiersLength();
        void *response = (void *)adapter.GetRadioAccessSpecifier();
        int resLen = specifiersLength*(sizeof(RIL_RadioAccessSpecifier_V1_5));
        OnRequestComplete(RIL_E_SUCCESS, response, resLen);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetVoNREnabledHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetVoNREnabledHandler, MSG_NET_SET_VONR_ENABLED)

int SetVoNREnabledHandler::OnRequest(Message *msg) {
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

    int enabled = rildata->GetInt();
    RilLogV("[%s] %s() VoNr Capability=%d", mServiceName, __FUNCTION__, enabled);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetVonrCapa(enabled);
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetVoNREnabledHandler::OnResponse(Message *msg) {
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
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * GetVoNREnabledHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetVoNREnabledHandler, MSG_NET_GET_VONR_ENABLED)

int GetVoNREnabledHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetVonrCapa();
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetVoNREnabledHandler::OnResponse(Message *msg) {
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

    ProtocolNetGetVonrCapaAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int capa = adapter.GetVonrCapa();
        OnRequestComplete(RIL_E_SUCCESS, &capa, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetUsageSettingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetUsageSettingHandler, MSG_NET_SET_USAGE_SETTING)

int SetUsageSettingHandler::OnRequest(Message *msg) {
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

    radio_network::V2_0::RIL_UsageSetting usageSetting = rildata->GetInt();
    RilLogV("[%s] %s() Usage Setting=%d", mServiceName, __FUNCTION__, (int32_t)usageSetting.value());
    if (!usageSetting.valid()) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetDeviceService((int32_t)usageSetting.value());
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetUsageSettingHandler::OnResponse(Message *msg) {
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
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * GetUsageSettingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetUsageSettingHandler, MSG_NET_GET_USAGE_SETTING)

int GetUsageSettingHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetDeviceService();
    if (SendRequest(pModemData, TIMEOUT_MISC_NETWORK_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetUsageSettingHandler::OnResponse(Message *msg) {
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

    ProtocolNetGetDeviceServiceAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int setting = adapter.GetUsageSetting();
        OnRequestComplete(RIL_E_SUCCESS, &setting, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}
