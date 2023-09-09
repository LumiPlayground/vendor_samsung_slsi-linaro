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
 * networkhandler.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#include "networkhandler.h"
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <sstream>
#include "netdatabuilder.h"
#include "eonsResolver.h"
#include "networkserviceproxy.h"
#include "networkutils.h"
#include "cellinfodecorator.h"
#include "protocoladapter.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "service.h"
#include <librilutils/systemproperty.h>
#include "util/util.h"
#include <unistd.h>

#define TIMEOUT_NET_DEFAULT                     30000
#define TIMEOUT_NET_5SEC                        5000
#define TIMEOUT_NET_ALLOW_DATA                  20000
#define TIMEOUT_NET_MICRO_CELL_SEARCH           50000
#define TIMEOUT_NET_RADIO_CAPABILITY            5000
#define TIMEOUT_NET_EMERGENCY_CALL              30000
#define TIMEOUT_NET_SET_NETWORK_MODE_AUTO       60000 // 1 min.
#define TIMEOUT_NET_NR_MODE                     5000

#define REASON_QUERY_PS_DOMAIN_STATE                    "reason_query_ps_domain_state"
#define REASON_ALLOW_DATA                               "reason_allow_data"
#define REASON_SET_DS_NETWORK_TYPE                      "reason_set_ds_network_type"
#define REASON_SET_DUAL_NETWORK_TYPE_AND_ALLOW_DATA     "reason_set_dual_network_type_and_allow_data"

#define MCC_BRAZIL      (724)

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

static void SetAllowDataState(bool allowed, const char *reason, int phoneId)
{
    RilLogV("[%s] allowed=%d reason=%s phoneId=%d", __FUNCTION__, allowed, reason, phoneId);
    // vendor_rild_prop
    const char *propName = "vendor.ril.allow_data_";
    stringstream ss;
    ss << propName << phoneId;
    SystemProperty::Set(ss.str(), allowed ? 1 : 0);
}

/**
 * GetVoiceRadioTechHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetVoiceRadioTechHandler, MSG_NET_VOICE_RADIO_TECH)

int GetVoiceRadioTechHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int voiceRat = network.GetVoiceRadioTech();
    int voiceRegState = network.GetVoiceRegistrationState();
    if (voiceRat == RADIO_TECH_UNKNOWN ||
        !(voiceRegState == REGISTERED_HOME || voiceRegState == REGISTERED_ROAMING)) {
        RilLogI("[%s] %s() Voice Radio Tech is unknown or not registered", mServiceName, __FUNCTION__);
        voiceRat = RADIO_TECH_UNKNOWN;
        OnRequestComplete(RIL_E_SUCCESS, &voiceRat, sizeof(int));
        return 0;
    }

    OnRequestComplete(RIL_E_SUCCESS, &voiceRat, sizeof(int));
    return 0;
}

int GetVoiceRadioTechHandler::OnResponse(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}

/**
 * SetBandModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetBandModeHandler, MSG_NET_SET_BAND_MODE)

int SetBandModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int bandMode = rildata->GetInt();
    RilLogV("Set Bandmode=%d", bandMode);
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetBandMode(bandMode);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetBandModeHandler::OnResponse(Message *msg)
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
 * QueryAvailableBandModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(QueryAvailableBandModeHandler, MSG_NET_QUERY_AVAILABLE_BAND_MODE)

int QueryAvailableBandModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildQueryAvailableBandMode();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }
    return 0;
}

int QueryAvailableBandModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetBandModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        const int *bandMode = adapter.GetAvialableBandMode();
        int count = adapter.GetCount();

        RilLogV("Available Bandmode count=%d", count);

        NetworkDataBuilder builder;
        auto rildata = builder.BuildNetAvailableBandModeResponse(bandMode, count);
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_MODEM_ERR);
        }
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * AllowDataModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(AllowDataModeHandler, MSG_NET_ALLOW_DATA)

int AllowDataModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = static_cast<IntRequestData *>(msg->GetRequestData());
    int state = rildata->GetInt();
    RilLogV("%s", state == ALLOW_DATA_CALL ? "ALLOW_DATA_CALL" : "DISALLOW_DATA_CALL");

    // For dual volte.
    bool isDualVoLTE = false;
    string val = SystemProperty::Get(RIL_VENDOR_RADIO_DUAL_VOLTE, "");
    if (TextUtils::Equals(val.c_str(), "1")) {
        isDualVoLTE = true;
    }

    RilProperty *pRilProperty = mService->GetRilApplicationProperty();
    // overallCardState
    // bit 0 : SIM1 card state, bit 1 : SIM2 card state
    // 0 : No Sim, 1 : Only SIM1, 2 : Only SIM2, 3 = Dual SIM
    int overallCardState = pRilProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
    RilLogV("overallCardState=0x%02X", overallCardState);

    // If it supports dual volte, it doesn't have to wait.
    if (state == ALLOW_DATA_CALL && overallCardState == 0x3 && !isDualVoLTE) {
        unsigned int waitTimeMs = 100;    // 100 ms
        stringstream ss;
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << (mService->GetRilSocketId() + 1) % 2;;
        string strPropName = ss.str();
        bool enabled = false;
        int count = (TIMEOUT_NET_ALLOW_DATA / waitTimeMs) + 10;
        RilLogW("[%d] Waiting PS Service available", mService->GetRilSocketId());
        while (count-- > 0) {
            enabled = pRilProperty->GetBool(strPropName);
            if (!enabled) {
                break;
            }
            WaitForTimeUs(__FUNCTION__, waitTimeMs * 1000U);
        } // end while ~
        RilLogV("[%d] GetBool(%s)=%d", mService->GetRilSocketId(), strPropName.c_str(), enabled);
    }
    if (state == DISALLOW_DATA_CALL && overallCardState != 0x3) {
        // Just consider as completion. CP will always return fails and no PS reg
        OnRequestComplete(RIL_E_SUCCESS);
        return 0;
    }

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetAllowData(state);
    if (SendRequest(pModemData, TIMEOUT_NET_ALLOW_DATA, mMessageId) < 0) {
        return -1;
    }

    // set default data subscription PhoneID
    //update ACTIVE PS data SIM
    if (state == ALLOW_DATA_CALL) {
        RilLogV("[%s] Update Active data SIM slot{%s=%d}", __FUNCTION__, RIL_APP_PS_ACTIVE_SIM, mService->GetRilSocketId());
        pRilProperty->Put(RIL_APP_PS_ACTIVE_SIM, mService->GetRilSocketId());

        if (mRilContext->GetUiccCardState() == RIL_CARDSTATE_PRESENT) {
            RilLogV("[%s] SIM card present: update registration state", mServiceName);
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
        }
    }
    return 0;
}

int AllowDataModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    RilProperty *pRilProperty = mService->GetRilApplicationProperty();
    if (errorCode == RIL_E_SUCCESS) {
        IntRequestData *rildata = (IntRequestData *)mService->GetCurrentMsg()->GetRequestData();
        int state = rildata->GetInt();
        stringstream ss;
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << mService->GetRilSocketId();;
        string strPropName = ss.str();
        pRilProperty->Put(strPropName, (state == ALLOW_DATA_CALL));
        RilLogV("Put(%s, %d)", strPropName.c_str(), state);
        WaitForTimeUs(__FUNCTION__, 500'000U);

        pRilProperty->Put(RIL_APP_PS_ALLOW_SYNCDONE, 1);
        OnRequestComplete(RIL_E_SUCCESS);

        // update allowDataState in system
        SetAllowDataState((state == ALLOW_DATA_CALL), REASON_ALLOW_DATA,  mService->GetRilSocketId());
    }
    else {
        // HAL expects RadioError:NONE at all times, and it expects processing correctly someday.
        // All error from CP should be processed internally, and make it correct at last
        // property->Put(RIL_APP_PS_ACTIVE_SIM, GetRilSocketId()); will be there.
        // Need to retry with that value
        int overallCardState = pRilProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
        int isPresent = (overallCardState & (0x01<<mService->GetRilSocketId())) >> mService->GetRilSocketId();
        // ErrorCase 1: Current Socket is not SIM CARD PRESENT
        RilLogV("SOCKET:%d is has card state:%d(overall:%d)", mService->GetRilSocketId(), isPresent, overallCardState);
        // Check Actual DataAllow State from CP
        // Queueing DataAllow request to sync up Framework's request and CP state

        if (mRilContext->GetUiccCardState() == RIL_CARDSTATE_PRESENT) {
            RilLogV("[%s] SIM card present and received failed", mServiceName);
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
        else
        {
            RilLogV("[%s] SIM card absent or PIN locked, just ignore", mServiceName);
            OnRequestComplete(RIL_E_SUCCESS);
        }
    }

    return 0;
}

/**
 * SetUnsolCellInfoListRateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetUnsolCellInfoListRateHandler, MSG_NET_SET_UNSOL_CELL_INFO_LIST_RATE)

int SetUnsolCellInfoListRateHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = static_cast<IntRequestData *>(msg->GetRequestData());
    int rate = rildata->GetInt();
    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetCellInfoListReportRate(rate);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetUnsolCellInfoListRateHandler::OnResponse(Message *msg)
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
 * GetDuplexModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetDuplexModeHandler, MSG_NET_GET_DUPLEX_MODE)

int GetDuplexModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    RilProperty *property = mService->GetRilApplicationProperty();
    int curServiceNum = mService->GetRilSocketId();
    int socketid = property->GetInt(RIL_APP_PS_ACTIVE_SIM, -1);
    int cardState = mRilContext->GetUiccCardState();
    RilLogV("get duplex curServiceNum(%d), socketid(%d), card_state(%d)\n", curServiceNum, socketid, cardState);
    if((curServiceNum == socketid) && (cardState == RIL_CARDSTATE_PRESENT)) {
        ProtocolNetworkModeBuilder builder;
        ModemData *pModemData = builder.BuildGetDuplexMode();
        if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
            return -1;
        }
        return 0;
    } else {
        RilLogE("Get Duplex mode no main sim");
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    return 0;
}

int GetDuplexModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    int mode = 0;
    ProtocolNetDuplexModeRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int duplex_mode_4g = (int)adapter.Get4gDuplexMode();
        int duplex_mode_3g = (int)adapter.Get3gDuplexMode();
        RilLogV("Get Duplex mode done 4g=%d, 3g=%d", duplex_mode_4g, duplex_mode_3g);

        mode = adapter.GetDuplexMode();
        if ( mode == DUPLEX_MODE_INVALID ) {
            RilLogE("Get Duplex mode wrong value 4g=%d, 3g=%d", duplex_mode_4g, duplex_mode_3g);
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
        else {
            OnRequestComplete(RIL_E_SUCCESS, &mode, sizeof(int));
        }
    }
    else {
        RilLogE("Get Duplex mode error code(%d)", errorCode);
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * SetDuplexModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDuplexModeHandler, MSG_NET_SET_DUPLEX_MODE)

int SetDuplexModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData* pReq = (IntRequestData*)msg->GetRequestData();
    RilProperty *property = mService->GetRilApplicationProperty();
    int curServiceNum = mService->GetRilSocketId();
    int socketid = property->GetInt(RIL_APP_PS_ACTIVE_SIM, -1);
    int cardState = mRilContext->GetUiccCardState();
    int mode = pReq->GetInt();
    RilLogV("SIM%d - Set Duplex mode=%d", curServiceNum, mode);

    ProtocolNetworkModeBuilder builder;
    if((curServiceNum == socketid) && (cardState == RIL_CARDSTATE_PRESENT)) {
        ModemData *pModemData = builder.BuildSetDuplexMode(mode);
        if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT*2, mMessageId) < 0) {
            return -1;
        }
        return 0;
    } else {
        RilLogE("Set Duplex mode no main sim\n");
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    return 0;
}

int SetDuplexModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetDuplexModeRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RilLogV("Set Duplex mode done\n");
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        RilLogE("Set Duplex mode error code(%d)", errorCode);
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * QueryNetworkSelectionModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(QueryNetworkSelectionModeHandler, MSG_NET_QUERY_NETWORK_SELECTION_MODE)

int QueryNetworkSelectionModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    int radioState = mRilContext->GetCurrentRadioState();
    if ( radioState == RADIO_STATE_OFF
        || radioState == RADIO_STATE_UNAVAILABLE ) {

        NetworkDataBuilder builder;
        auto rildata = builder.BuildNetSelectModeResponse(0);
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
        }
        return 0;
    }

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildQueryNetworkSelectionMode();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int QueryNetworkSelectionModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetSelModeAdapter adapter(pModemData);
    int netSelectionMode = adapter.GetNetworkSelectionMode();

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        NetworkDataBuilder builder;
        auto rildata = builder.BuildNetSelectModeResponse(netSelectionMode);
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetNetworkSelectionAutoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetNetworkSelectionAutoHandler, MSG_NET_SET_NETWORK_SELECTION_AUTO)

int SetNetworkSelectionAutoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    RilProperty *pRilProperty = mService->GetRilApplicationProperty();
    // overallCardState
    // bit 0 : SIM1 card state, bit 1 : SIM2 card state
    // 0 : No Sim, 1 : Only SIM1, 2 : Only SIM2, 3 = Dual SIM
    int overallCardState = pRilProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
    RilLogV("DoSetNetworkSelectionAuto overallCardState=0x%02X", overallCardState);
    if (overallCardState == 0) {
        OnRequestComplete(RIL_E_OPERATION_NOT_ALLOWED);
        return 0;
    }

/*
    KAN8895-2010 : CP team ask to remove this blocking codes.

    UINT nRet = IsOppsiteStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during other stack is busy", m_szSvcName, __FUNCTION__);
        return -1;
    }

#ifdef RIL_FEATURE_FUNCTION_CHECK_CURRENT_STACK_BUSY
    nRet = IsCurrentStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during stack is busy", m_szSvcName, __FUNCTION__);
        return -1;
    }
#endif
*/
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetNetworkSelectionAuto();
    if (SendRequest(pModemData, TIMEOUT_NET_SET_NETWORK_MODE_AUTO, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetNetworkSelectionAutoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS)
        OnRequestComplete(RIL_E_SUCCESS);
    else
        OnRequestComplete(RIL_E_OPERATION_NOT_ALLOWED);
    return 0;
}

/**
 * GetCellInfoListHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCellInfoListHandler, MSG_NET_GET_CELL_INFO_LIST)

int GetCellInfoListHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetCellInfoList();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int GetCellInfoListHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetCellInfoListAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RilProperty *prop = mService->GetRilApplicationProperty();
        int cellInfoVersion = prop->GetInt("CellInfo", DEFAULT_CELL_INFO_VER);
        list<RIL_CellInfo_V1_6> &cellInfoList = adapter.GetCellInfoList(cellInfoVersion);

        int halVer = RilApplication::RIL_HalVersionCode;
        CellInfoListBuilder builder(halVer);
        const RilData *rildata = builder.Build(cellInfoList);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
        else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * CellInfoListReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CellInfoListReceivedHandler, MSG_NET_CELL_INFO_LIST_RECEIVED)

int CellInfoListReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetCellInfoListIndAdapter adapter(pModemData);
    int halVer = RilApplication::RIL_HalVersionCode;
    CellInfoListBuilder builder(halVer);
    RilProperty *prop = mService->GetRilApplicationProperty();
    int cellInfoVersion = prop->GetInt("CellInfo", DEFAULT_CELL_INFO_VER);
    const RilData *rildata = builder.Build(adapter.GetCellInfoList(cellInfoVersion));
    if (rildata != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_CELL_INFO_LIST, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    return 0;
}

/**
 * SetDSNetworkTypeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDSNetworkTypeHandler, MSG_NET_SET_DS_NTW_TYPE)

int SetDSNetworkTypeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int netType = rildata->GetInt();
    RilLogI("DualSim Network Type : %d", netType);

    ProtocolNetworkTypeBuilder builder;
    ModemData *pModemData = builder.BuildSetDSNetworkType(netType);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    //update ACTIVE PS data SIM
    RilProperty *property = mService->GetRilApplicationProperty();
    int phoneId = mService->GetRilSocketId();
    int cardState = mRilContext->GetUiccCardState();
    if (property != NULL) {
        RilLogV("[%s] Update Active data SIM slot{%s=%d}", __FUNCTION__, RIL_APP_PS_ACTIVE_SIM, phoneId);
        property->Put(RIL_APP_PS_ACTIVE_SIM, phoneId);
        property->Put(RIL_APP_MAIN_SIM, phoneId);

        if (cardState == RIL_CARDSTATE_PRESENT) {
            RilLogV("[%s] SIM card present: update registration state", mServiceName);
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
        }
    }

    return 0;
}

int SetDSNetworkTypeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);

        RilProperty *property = mService->GetRilApplicationProperty();
        int phoneId = mService->GetRilSocketId();

        stringstream ss;
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << phoneId;
        string strPropName = ss.str();
        property->Put(strPropName, true);
        RilLogV("Put(%s, %d)", strPropName.c_str(), true);

        // set false to opposit sim.
        ss.str("");
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << (phoneId + 1) % 2;
        strPropName = ss.str();
        property->Put(strPropName, false);

        // update allowDataState in system
        // Active PS status by SetDSNetworkType should be exclusive
        SetAllowDataState(true, REASON_SET_DS_NETWORK_TYPE, phoneId);
        SetAllowDataState(false, REASON_SET_DS_NETWORK_TYPE, (phoneId + 1) % 2);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * GetPreferredNetworkTypeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPreferredNetworkTypeHandler, MSG_NET_GET_PREF_NETWORK_TYPE)

int GetPreferredNetworkTypeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkTypeBuilder builder;
    ModemData *pModemData = builder.BuildGetPreferredNetworkType();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetPreferredNetworkTypeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetPrefNetTypeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int netType = adapter.GetPreferredNetworkType();
        int rafBitmap = RadioConvUtils::getRafFromNetworkType(netType);
        RilLogV("[%d] Pref NetType(%d), NetworkTypeBitmap=0x%02X", mService->GetRilSocketId(), netType, rafBitmap);
        OnRequestComplete(RIL_E_SUCCESS, &rafBitmap, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetPreferredNetworkTypeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPreferredNetworkTypeHandler, MSG_NET_SET_PREF_NETWORK_TYPE)

int SetPreferredNetworkTypeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    UINT nRet = network.IsOppsiteStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during other stack is busy", mServiceName, __FUNCTION__);
        return -1;
    }

#ifdef RIL_FEATURE_FUNCTION_CHECK_CURRENT_STACK_BUSY
    nRet = network.IsCurrentStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during stack is busy", mServiceName, __FUNCTION__);
        return -1;
    }
#endif

    int phoneId = mService->GetRilSocketId();
    int netType = rildata->GetInt();
    int halVer = rildata->GetHalVersion();
    if (halVer >= HAL_VERSION_CODE(1, 4)) {
        int netTypeBitmap = netType;
        RilLogV("[%d] NetworkTypeBitmap=0x%02X", phoneId, netTypeBitmap);
        netType = RadioConvUtils::getNetworkTypeFromRaf(netTypeBitmap);
    }
    RilLog("Preferred Network Type : %d", netType);

    ProtocolNetworkTypeBuilder builder;
    ModemData *pModemData = builder.BuildSetPreferredNetworkType(netType);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetPreferredNetworkTypeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        // VtsHalRadioV1_4Target#RadioHidlTest_v1_4.setPreferredNetworkTypeBitmap
        // an expected error code is always RIL_E_SUCCESS
        RilLogW("[%d] setPreferredNetworkType error by modem", mService->GetRilSocketId());
        OnRequestComplete(RIL_E_SUCCESS);
    }
    return 0;
}

/**
 * SetDualNetworkTypeAndAllowDataHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDualNetworkTypeAndAllowDataHandler, MSG_NET_SET_DUAL_NTW_AND_ALLOW_DATA)

int SetDualNetworkTypeAndAllowDataHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    UINT nRet = network.IsOppsiteStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during other stack is busy", mServiceName, __FUNCTION__);
        return -1;
    }

#ifdef RIL_FEATURE_FUNCTION_CHECK_CURRENT_STACK_BUSY
    nRet = network.IsCurrentStackOccupyRF();
    if ( nRet != 0 )
    {
        RilLogE("[%s] %s() : Cannot change network setting during stack is busy", mServiceName, __FUNCTION__);
        return -1;
    }
#endif

    int typeForPrimary= rildata->GetInt(0);
    int typeForSecondary = rildata->GetInt(1);
    int allowedForPrimary = rildata->GetInt(2);
    int allowedForSecondary = rildata->GetInt(3);

    RilLog("Dual Network Type : Primary(%d,%d), Secondary(%d,%d)", typeForPrimary, allowedForPrimary, typeForSecondary, allowedForSecondary);

    ProtocolNetworkTypeBuilder builder;
    ModemData *pModemData = builder.BuildSetDualNetworkAndAllowData(typeForPrimary, typeForSecondary, allowedForPrimary, allowedForSecondary);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    //update Main SIM
    RilProperty *property = mService->GetRilApplicationProperty();
    if (property != NULL) {
        int phoneId = mService->GetRilSocketId();
        RilLogV("[%s] Update Main SIM slot{%s=%d}", __FUNCTION__, RIL_APP_MAIN_SIM, phoneId);
        property->Put(RIL_APP_PS_ACTIVE_SIM, phoneId);
        property->Put(RIL_APP_MAIN_SIM, phoneId);

        if (mRilContext->GetUiccCardState() == RIL_CARDSTATE_PRESENT) {
            RilLogV("[%s] SIM card present: update registration state", mServiceName);
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
        }
    }
    return 0;
}

int SetDualNetworkTypeAndAllowDataHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);

        int phoneId = mService->GetRilSocketId();
        RilProperty *property = mService->GetRilApplicationProperty();
        stringstream ss;
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << phoneId;
        string strPropName = ss.str();
        property->Put(strPropName, true);
        RilLogV("Put(%s, %d)", strPropName.c_str(), true);

        // set false to opposit sim.
        ss.str("");
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << (phoneId + 1) % 2;
        strPropName = ss.str();
        property->Put(strPropName, false);

        // update allowDataState in system
        // Active PS status by SetDualNetworkTypeAndAllowData should be exclusive
        SetAllowDataState(true, REASON_SET_DUAL_NETWORK_TYPE_AND_ALLOW_DATA, phoneId);
        SetAllowDataState(false, REASON_SET_DUAL_NETWORK_TYPE_AND_ALLOW_DATA, (phoneId + 1) % 2);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * StartNetworkScanHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StartNetworkScanHandler, MSG_NET_START_NETWORK_SCAN)

int StartNetworkScanHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg)

    NetworkScanReqData *rildata = (NetworkScanReqData *)msg->GetRequestData();
    ProtocolNetworkScanBuilder builder;
    int scanType = rildata->GetScanType();
    int timeInterval = rildata->GetTimeInterval();
    int specifiersLength = rildata->GetSpecifiersLength();
    RIL_RadioAccessSpecifier_V1_5 *specifiers =  rildata->GetRadioAccessSpecifier();
    int maxSearchTime = rildata->GetMaxSearchTime();
    bool incrementalResults = rildata->GetIncrementalResults();
    int incrementalResultsPeriodicity = rildata->GetIncrementalResultsPeriodicity();
    int numOfMccMncs = rildata->GetNumOfMccMncs();
    char **mccMncs = rildata->GetMccMncs();

    RilLog("scanType=%d timeInterval=%d specifiersLength=%d",
            scanType, timeInterval, specifiersLength);
    RilLog("maxSearchTime=%d incrementalResults=%d incrementalResultsPeriodicity=%d numOfMccMncs=%d",
            maxSearchTime, incrementalResults, incrementalResultsPeriodicity, numOfMccMncs);
    ModemData *pModemData = builder.BuildStartNetworkScan(scanType, timeInterval, specifiersLength, specifiers,
            maxSearchTime, incrementalResults, incrementalResultsPeriodicity,
            numOfMccMncs, mccMncs);
    if (pModemData == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    // timeout : maxSearchTime + 5 sec
    if (SendRequest(pModemData, ((maxSearchTime + 5) * 1000), mMessageId) < 0) {
        OnRequestComplete(RIL_E_OPERATION_NOT_ALLOWED);
        return 0;
    }
    return 0;
}

int StartNetworkScanHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    UINT errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        // VTS_1_2 allowed error code: RadioError::NONE, RadioError::REQUEST_NOT_SUPPORTED
        errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
        if (mService->GetCurrentRequestData() != NULL) {
            NetworkScanReqData *nsr = (NetworkScanReqData *)mService->GetCurrentRequestData();
            int halVer = nsr->GetHalVersion();
            bool isLegacyRequest = nsr->IsLegacyRequest();
            RilLogV("HAL_VERSION_CODE=%X IsLegacyReques=%d", halVer, isLegacyRequest);
            if (halVer >= HAL_VERSION_CODE(1,4)) {
                if (isLegacyRequest) {
                    // Allowed error codes by VTS_1_4 RadioHidlTest_v1_4#startNetworkScan
                    //   RadioError::NONE, RadioError::OPERATION_NOT_ALLOWED
                    errorCode = RIL_E_OPERATION_NOT_ALLOWED;
                }
                else {
                    // Allowed error codes by VTS_1_4 RadioHidlTest_v1_4#startNetworkScan_GoodRequestX
                    //   RadioError::NONE, RadioError::INVALID_ARGUMENTS
                    errorCode = RIL_E_INVALID_ARGUMENTS;
                }
            }

            // For CTS test(CtsCarrierApiTestCases::testRequestNetworkScan)
            // If CP doesn't support a network scan feature,
            // RIL will send a network scan start response as SUCCESS
            // and a network scan result indication as COMPLETE.
            OnRequestComplete(RIL_E_SUCCESS, NULL, 0);

            RIL_NetworkScanResult_V1_4 result;
            memset(&result, 0, sizeof(result));
            result.status = (RIL_ScanStatus)COMPLETE;
            result.error = (RIL_Errno)RIL_E_REQUEST_NOT_SUPPORTED;
            int unsolResponse = ENCODE_REQUEST(RIL_UNSOL_NETWORK_SCAN_RESULT, halVer);
            OnUnsolicitedResponse(unsolResponse, &result, sizeof(result));
            return 0;
        }
    }
    OnRequestComplete(errorCode, NULL, 0);
    return 0;
}

/**
 * StopNetworkScanHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StopNetworkScanHandler, MSG_NET_STOP_NETWORK_SCAN)

int StopNetworkScanHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkScanBuilder builder;
    ModemData *pModemData = builder.BuildStopNetworkScan();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0 ) {
        return -1;
    }
    return 0;
}

int StopNetworkScanHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) errorCode = RIL_E_REQUEST_NOT_SUPPORTED;
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * NetworkScanResultHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NetworkScanResultHandler, MSG_NET_IND_NETWORK_SCAN_RESULT)

int NetworkScanResultHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int phoneId = mService->GetRilSocketId();
    ProtocolNetScanResultAdapter adapter(pModemData);
    int status = adapter.GetScanStatus();
    int errorCode = adapter.GetScanResult();

    int halVer = RilApplication::RIL_HalVersionCode;
    NetworkScanResultBuilder builder(halVer);

    std::string simNumeric = GetSimOperatorNum(phoneId);
    std::string spn = GetSimSpn(phoneId);

    RilProperty *prop = mService->GetRilApplicationProperty();
    int cellInfoVersion = prop->GetInt("CellInfo", DEFAULT_CELL_INFO_VER);
    list<RIL_CellInfo_V1_6> &cellInfoList = adapter.GetCellInfoList(cellInfoVersion);
    list<RIL_CellInfo_V1_6>::iterator iter;
    size_t i = 0;
    for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++) {
        CellInfoBase cellinfo(&(*iter));
        SignalStrengthVisibleDecorator visible(cellinfo);
        AlphaEnosDecorator alpha(visible, simNumeric, spn, EonsResolver::createForPhoneId(mPhoneId));
        CellInfoDecorator& decorator = alpha;
        decorator.decorate();
        RilLog("[%zu] %s", i++, NetworkUtils::toString(*iter).c_str());
    }

    const RilData *rildata = builder.Build(status, errorCode, cellInfoList);
    if (rildata != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_NETWORK_SCAN_RESULT, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    return 0;
}

/**
 * SetEndcModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEndcModeHandler, MSG_NET_OEM_SET_ENDC_MODE)

int SetEndcModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt();
    RilLogV("[%s] %s() mode=%d", mServiceName, __FUNCTION__, mode);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetEndcMode(mode);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetEndcModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * GetEndcModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetEndcModeHandler, MSG_NET_OEM_GET_ENDC_MODE)

int GetEndcModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetEndcMode();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetEndcModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetEndcModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int mode = adapter.GetEndcMode();
        OnRequestComplete(RIL_E_SUCCESS, &mode, sizeof(int));
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * SetNrModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetNrModeHandler, MSG_NET_SET_NR_MODE)

int SetNrModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt();
    RilLogV("[%s] %s() mode=%d", mServiceName, __FUNCTION__, mode);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetNrMode(mode);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetNrModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
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
 * GetNrModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetNrModeHandler, MSG_NET_GET_NR_MODE)

int GetNrModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetNrMode();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetNrModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetNrModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int mode = adapter.GetNrMode();
        OnRequestComplete(RIL_E_SUCCESS, &mode, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetNrSilenceModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetNrSilenceModeHandler, MSG_NET_SET_NR_SILENCE_MODE)

int SetNrSilenceModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    if (rildata->GetSize() < 2) {
        RilLogE("Invalid parameters");
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    int nsa = rildata->GetInt(0);
    int sa = rildata->GetInt(1);
    RilLogV("[%d] NSA silence %d SA silence %d", mService->GetRilSocketId(), nsa, sa);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetNrSilenceMode(nsa, sa);
    if (SendRequest(pModemData, TIMEOUT_NET_NR_MODE, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetNrSilenceModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        errorCode = RIL_E_MODEM_ERR;
    }
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * GetNrSilenceModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetNrSilenceModeHandler, MSG_NET_GET_NR_SILENCE_MODE)

int GetNrSilenceModeHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetNrSilenceMode();
    if (SendRequest(pModemData, TIMEOUT_NET_NR_MODE, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetNrSilenceModeHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetNrSilenceModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int results[2] = { adapter.GetNsaMode(), adapter.GetSaMode() };
        RilLogV("[%d] NSA mode=%d SA mode=%d", mPhoneId, results[0], results[1]);
        OnRequestComplete(RIL_E_SUCCESS, results, sizeof(int) * 2);
    } else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * SetPsServiceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPsServiceHandler, MSG_NET_OEM_SET_PS_SERVICE)

int SetPsServiceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = static_cast<IntRequestData *>(msg->GetRequestData());
    int state = rildata->GetInt();
    RilLogV("%s", state == ALLOW_DATA_CALL ? "ALLOW_DATA_CALL" : "DISALLOW_DATA_CALL");

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetAllowData(state);
    if (SendRequest(pModemData, TIMEOUT_NET_ALLOW_DATA, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetPsServiceHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

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
 * GetPsServiceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPsServiceHandler, MSG_NET_OEM_GET_PS_SERVICE)

int GetPsServiceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetPsService();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetPsServiceHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetPsServiceAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int state = adapter.GetState();
        RilLogV("state=%s", state == ALLOW_DATA_CALL ? "ALLOW_DATA_CALL" : "DISALLOW_DATA_CALL");
        // set default data subscription PhoneID
        //update ACTIVE PS data SIM
        RilProperty *property = mService->GetRilApplicationProperty();
        int phoneId = mService->GetRilSocketId();
        if (property != NULL && (state == ALLOW_DATA_CALL)) {
            property->Put(RIL_APP_PS_ACTIVE_SIM, phoneId);
            RilLogV("RilAppliationProperty: Put{%s=%d}", RIL_APP_PS_ACTIVE_SIM, phoneId);

            if (mRilContext->GetUiccCardState() == RIL_CARDSTATE_PRESENT) {
                RilLogV("[%s] SIM card present: update registration state", mServiceName);
                OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
            }
        }
        OnRequestComplete(RIL_E_SUCCESS, &state, sizeof(int));

        // update allowDataState in system
        SetAllowDataState(state == ALLOW_DATA_CALL, REASON_QUERY_PS_DOMAIN_STATE, phoneId);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * SetMicroCellSearchHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetMicroCellSearchHandler, MSG_NET_SET_MC_SRCH)

int SetMicroCellSearchHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData* pReq = (IntRequestData*)msg->GetRequestData();
    int mode = pReq->GetInt();
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetMicroCellSearch((BYTE)mode);
    if (SendRequest(pModemData, TIMEOUT_NET_MICRO_CELL_SEARCH, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetMicroCellSearchHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetMcSrchRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        const int srch_result = adapter.GetMcSrchResult();
        const char *plmn = adapter.GetMcSrchPlmn();

        RilLogV("Micro Cell search {result=%d, PLMN=%s}", srch_result, plmn);

        NetworkDataBuilder builder;
        auto rildata = builder.BuildNetFemtoCellSrchResponse(srch_result, plmn);
        if (rildata != nullptr && srch_result >= 0) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

int SetMicroCellSearchHandler::OnTimeout(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    // Cancel requested Micro cell searching
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetMicroCellSearch((BYTE)RIL_MC_SRCH_MODE_CANCEL);
    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) {
            RilLogW("Fail to send Cancel micro cell search request ");
        }
        delete pModemData;
    }

    // explicitly error
    OnRequestComplete(RIL_E_GENERIC_FAILURE);
    return 0;
}

/**
 * GetRadioCapabilityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetRadioCapabilityHandler, MSG_NET_GET_RC_NTW_TYPE)

int GetRadioCapabilityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetRCNetworkType();
    if (SendRequest(pModemData, TIMEOUT_NET_RADIO_CAPABILITY, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetRadioCapabilityHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    int phoneId = mService->GetRilSocketId();
    ProtocolGetNetworkRCRespAdapter adapter(pModemData);
    RIL_RadioCapability resp;
    memset(&resp, 0, sizeof(resp));
    resp.version = RIL_RADIO_CAPABILITY_VERSION;
    resp.session = 0;
    resp.phase = RC_PHASE_CONFIGURED;
    resp.rat = NetworkUtils::getDefaultRAF();
    snprintf(resp.logicalModemUuid, sizeof(resp.logicalModemUuid) - 1, "exynos_modem%d", phoneId);
    resp.status = RC_STATUS_NONE;

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        resp.rat = adapter.GetRafType();
        RilLogW("Use modem RAF!!!");
    }
    else {
        RilLogW("Use default RAF by RIL!!!");
    }

    int testRaf = NetworkUtils::getTestRAF(phoneId);
    if (testRaf > 0) {
        resp.rat = testRaf;
        RilLogV("===== test raf %d(%X) =====", testRaf, testRaf);
    }
    NetworkUtils::printRafDetail(resp.rat);

    OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(RIL_RadioCapability));
    return 0;
}

int GetRadioCapabilityHandler::OnTimeout(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    // error handling
    // all RAT available in default
    int phoneId = mService->GetRilSocketId();
    RIL_RadioCapability resp;
    memset(&resp, 0, sizeof(resp));
    resp.version = RIL_RADIO_CAPABILITY_VERSION;
    resp.session = 0;
    resp.phase = RC_PHASE_CONFIGURED;
    resp.rat = NetworkUtils::getDefaultRAF();
    snprintf(resp.logicalModemUuid, sizeof(resp.logicalModemUuid) - 1, "exynos_modem%d", phoneId);
    resp.status = RC_STATUS_NONE;
    RilLogW("Use default RAF by RIL!!!(timeout)");

    OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(RIL_RadioCapability));
    return 0;
}

/**
 * SetRadioCapabilityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetRadioCapabilityHandler, MSG_NET_SET_RC_NTW_TYPE)

int SetRadioCapabilityHandler::OnRequest(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    return 0;
}

int SetRadioCapabilityHandler::OnResponse(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    return 0;
}

/**
 * RadioCapabilityChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RadioCapabilityChangedHandler, MSG_NET_RC_INFO_RECV)

int RadioCapabilityChangedHandler::OnIndication(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return 0;
}

/**
 * TotalOostHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TotalOostHandler, MSG_NET_IND_TOTAL_OOS)

int TotalOostHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    OnUnsolicitedResponse(RIL_UNSOL_OEM_TOTAL_OOS);

    return 0;
}

/**
 * MccIndtHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(MccIndtHandler, MSG_NET_IND_MCC)

int MccIndtHandler::OnIndication(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return 0;
}

/**
 * SetSysSelChannelsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSysSelChannelsHandler, MSG_NET_SET_SYSTEM_SELECTION_CHANNELS)

int SetSysSelChannelsHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    SystemSelectionChannelsReqData *rilData = (SystemSelectionChannelsReqData *)msg->GetRequestData();
    ProtocolNetworkModeBuilder builder;
    bool isSpecifyChannel = rilData->GetIsSpecifyChannels();
    int specifiersLength = rilData->GetSpecifiersLength();
    RIL_RadioAccessSpecifier_V1_5 *pSpecifiers =  rilData->GetRadioAccessSpecifier();
    RilLogI("isSpecifyChannel=%d specifiersLength=%d", isSpecifyChannel, specifiersLength);

    ModemData *pModemData = builder.BuildSetSysSelChannels(isSpecifyChannel, specifiersLength, pSpecifiers);
    if (SendRequest(pModemData, TIMEOUT_NET_5SEC, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetSysSelChannelsHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    UINT errorCode = adapter.GetErrorCode();
    if (errorCode != RIL_E_SUCCESS) {
        // VTS_1_5 allowed error code: RadioError::NONE, RadioError::RADIO_NOT_AVAILABLE, RadioError::INTERNAL_ERR
        // CTS might expect success.
        RilLogI("[%s] %s(): errorCode = %d", mServiceName, __FUNCTION__, errorCode);
        errorCode = RIL_E_SUCCESS;
    }

    OnRequestComplete(errorCode);
    return 0;
}

/**
 * GetBarringInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetBarringInfoHandler, MSG_NET_GET_BARRING_INFO)

int GetBarringInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetBarringInfo();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetBarringInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    RilProperty *prop = mService->GetRilApplicationProperty();
    int cellIdentityVer = prop->GetInt(STRING_CELL_IDENTITY, DEFAULT_CELL_INFO_VER);
    ProtocolGetBarringInfoRspAdapter adapter(pModemData, cellIdentityVer);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RIL_CellIdAndBarringInfo *rspData = adapter.GetCellIdAndBarringInfo();

        // Indicates a restricted state change
        RestrictedStateBuilder restrictedStateBuilder;
        const RilData *restrictedState = restrictedStateBuilder.Build(rspData);
        if (restrictedState) {
            OnUnsolicitedResponse(RIL_UNSOL_RESTRICTED_STATE_CHANGED, restrictedState->GetData(),
                    restrictedState->GetDataLength());
            delete restrictedState;
        }

        OnRequestComplete(RIL_E_SUCCESS, rspData, sizeof(RIL_CellIdAndBarringInfo));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * BarringInfoChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(BarringInfoChangedHandler, MSG_NET_IND_BARRING_INFO)

void BarringInfoChangedHandler::OnCreate() {
    if (mService) {
        mService->addRadioStateListener(this);
    }
}

void BarringInfoChangedHandler::OnDestroy() {
    if (mService) {
        mService->removeRadioStateListener(this);
    }
}

void BarringInfoChangedHandler::OnRadioOffOrNotAvailable() {
    RilLogV("[%s %d] %s", TAG, mPhoneId, __FUNCTION__);
    // clear a restricted state
    int32_t state = RIL_RESTRICTED_STATE_NONE;
    OnUnsolicitedResponse(RIL_UNSOL_RESTRICTED_STATE_CHANGED, &state, sizeof(state));
}

int BarringInfoChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    RilProperty *prop = mService->GetRilApplicationProperty();
    int cellIdentityVer = prop->GetInt(STRING_CELL_IDENTITY, DEFAULT_CELL_INFO_VER);
    ProtocolBarringInfoChangedAdapter adapter(pModemData, cellIdentityVer);

    RIL_CellIdAndBarringInfo* rspData = adapter.GetCellIdAndBarringInfo();

    // Indicates a restricted state change
    RestrictedStateBuilder restrictedStateBuilder;
    const RilData *restrictedState = restrictedStateBuilder.Build(rspData);
    if (restrictedState) {
        OnUnsolicitedResponse(RIL_UNSOL_RESTRICTED_STATE_CHANGED, restrictedState->GetData(),
                restrictedState->GetDataLength());
        delete restrictedState;
    }

    // Indicates a barring info changed
    OnUnsolicitedResponse(RIL_UNSOL_BARRING_INFO_CHANGED, rspData, sizeof(RIL_CellIdAndBarringInfo));

    BarringInfoBuilder barringInfoBuilder;
    const RilData *barringInfo = barringInfoBuilder.Build(rspData);
    if (barringInfo) {
        OnUnsolicitedResponse(RIL_UNSOL_OEM_BARRING_INFO_LOCATION_SANTIZED, barringInfo->GetData(), barringInfo->GetDataLength());
        delete barringInfo;
    }

    return 0;
}

/**
 * UnsolRegistrationFailedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolRegistrationFailedHandler, MSG_NET_IND_REG_FAILED)

int UnsolRegistrationFailedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    RilProperty *prop = mService->GetRilApplicationProperty();
    int cellIdentityVer = prop->GetInt(STRING_CELL_IDENTITY, DEFAULT_CELL_INFO_VER);
    ProtocolRegistrationFailedAdapter adapter(pModemData, cellIdentityVer);

    RIL_RegistrationFailed *rspData = adapter.GetRegistrationFailed();
    OnUnsolicitedResponse(RIL_UNSOL_REGISTRATION_FAILED, rspData, sizeof(RIL_RegistrationFailed));

    return 0;
}

/**
 * QueryEmergencyCallAvailableRadioTechHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(QueryEmergencyCallAvailableRadioTechHandler, MSG_NET_QUERY_AVAILABLE_EMERGENCY_CALL_STATUS)

int QueryEmergencyCallAvailableRadioTechHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetEmergencyCallStatus(EMERGENCY_CALL_STATUS_START, RADIO_TECH_UNSPECIFIED);
    if (SendRequest(pModemData, TIMEOUT_NET_EMERGENCY_CALL, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int QueryEmergencyCallAvailableRadioTechHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode =adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * SetEmergencyCallStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEmergencyCallStatusHandler, MSG_NET_SET_EMERGENCY_CALL_STATUS)

int SetEmergencyCallStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();

    // TODO set correct status and rat info
    int status = rildata->GetInt(0);
    int rat = rildata->GetInt(1);
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetEmergencyCallStatus(status, rat);
    if (SendRequest(pModemData, TIMEOUT_NET_EMERGENCY_CALL, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetEmergencyCallStatusHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode =adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * EmergencyActInfoReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EmergencyActInfoReceivedHandler, MSG_NET_EMERGENCY_ACT_INFO_RECEIVED)

int EmergencyActInfoReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetEmergencyActInfoAdapter adapter(pModemData);
    int rat = adapter.GetRat();
    int actStatus = adapter.GetActStatus();
    //bool emcAvailable = (rat == RADIO_TECH_LTE || rat == RADIO_TECH_LTE_CA) &&
    //    (actStatus == EMERGENCY_CALL_AVAILABLE);
    RilLogV("RAT=%d actStatus=%d", rat, actStatus);

    // notify result to the IMS stack
    // WriteVolteEmcServiceStatus(false, emcAvailable, rat, false, true);

    int result[] = { rat, actStatus };
    OnUnsolicitedResponse(RIL_UNSOL_EMERGENCY_ACT_INFO, result, sizeof(int) * 2);

    return 0;
}

/**
 * SetAllowedNetworkTypeBitmapHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAllowedNetworkTypeBitmapHandler, MSG_NET_SET_ALLOWED_NETWORK)

int SetAllowedNetworkTypeBitmapHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = static_cast<IntRequestData *>(msg->GetRequestData());
    int networkTypeBitmap = rildata->GetInt();
    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetAllowedNetworkTypeBitmap(networkTypeBitmap);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetAllowedNetworkTypeBitmapHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

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
 * GetAllowedNetworkTypeBitmapHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetAllowedNetworkTypeBitmapHandler, MSG_NET_GET_ALLOWED_NETWORK)

int GetAllowedNetworkTypeBitmapHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetAllowedNetworkTypeBitmap();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int GetAllowedNetworkTypeBitmapHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetAllowNetworkAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        UINT32 rat = adapter.GetRat();
        OnRequestComplete(RIL_E_SUCCESS, &rat, sizeof(UINT32));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetPlmnListHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPlmnListHandler, MSG_NET_SET_PLMN_LIST)

int SetPlmnListHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata =(RawRequestData *)(msg->GetRequestData());
    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetPlmnList(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetPlmnListHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    OnRequestComplete(errorCode);

    return 0;
}

/**
 * GetPlmnListHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPlmnListHandler, MSG_NET_GET_PLMN_LIST)

int GetPlmnListHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildGetPlmnList();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int GetPlmnListHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}

/**
 * SetImsRegistrationStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetImsRegistrationStateHandler, MSG_NET_SET_IMS_REGISTRATION_STATE)

int SetImsRegistrationStateHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *) msg->GetRequestData();
    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int reg = rildata->GetInt(0);
    int type = rildata->GetInt(1);
    RilLogV("[%s] %s %s() reg=%d, type=%d", mServiceName, TAG, __FUNCTION__, reg, type);


    bool isWfcEnabledOld = network.IsIwlanEnabled();
    network.SetIwlanEnabled(type == NET_TYPE_IWLAN && reg == REGISTERED_HOME);
    if (network.IsIwlanEnabled() != isWfcEnabledOld) {
        RilLogV("[%d] WfcEnabled state changed %d -> %d", mService->GetRilSocketId(), isWfcEnabledOld, network.IsIwlanEnabled());
        RilLogV("[%d] current radioState=%d", mService->GetRilSocketId(), network.GetRadioState());
        OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
    }

    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int SetImsRegistrationStateHandler::OnResponse(Message * /* msg */) {
    return -1;
}

/**
 * SetMobileDataStateHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetMobileDataStateHandler, MSG_NET_SET_MOBILE_DATA_STATE)

int SetMobileDataStateHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *) msg->GetRequestData();
    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int mobileData = rildata->GetInt(0);
    int roamingData = rildata->GetInt(1);
    RilLogV("[%s] %s %s() mobileData=%d, roamingData=%d", mServiceName, TAG, __FUNCTION__, mobileData, roamingData);

    ProtocolNetworkBuilder builder;
    ModemData *pModemData = builder.BuildSetMobileDataState(mobileData, roamingData);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SetMobileDataStateHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    OnRequestComplete(errorCode);

    return 0;
}

/**
 * CurrentPhysicalChannelConfigsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CurrentPhysicalChannelConfigsHandler, MSG_NET_PHYSICAL_CHANNEL_CONFIGS_RECV)

int CurrentPhysicalChannelConfigsHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg)

    ProtocolNetPhysicalChannelConfigs adapter(msg->GetModemData());
    auto& pccList = adapter.GetConfigList();
    auto rildata = std::unique_ptr<const RilData>(
                        PhysicalChannelConfigsBuilder(RilApplication::RIL_HalVersionCode)
                                .Build(pccList));

    // explicitly indication for NR status
    OnUnsolicitedResponse(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG,
            rildata->GetData(), rildata->GetDataLength());

    // Notify PCC raw data to the RIL clients
    OnUnsolicitedResponse(RIL_UNSOL_OEM_PHYSICAL_CHANNEL_CONFIGS,
            adapter.GetParameter(), adapter.GetParameterLength());

    if (RilApplication::RIL_HalVersionCode <= HAL_VERSION_CODE(1, 5)) {
        // Notify current rrc state referring to a PCC size
        int isRrcCon = (pccList.size() > 0 ? 1 : 0);
        mRilContext->OnRequest(RIL_REQUEST_OEM_RRC_INFO, &isRrcCon, sizeof(int), nullptr);
    }

    return 0;
}

/**
 * DsdsStatusChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DsdsStatusChangedHandler, MSG_NET_IND_DSDS_STATUS)

int DsdsStatusChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    bool isPsRestricted = ProtocolNetDsdsStatusIndAdapter(pModemData).IsPsRestricted();
    RilProperty *prop = mService->GetRilContextProperty();
    if (prop) {
        prop->Put(RIL_CONTEXT_NET_PS_RESTRICTED, isPsRestricted);
    }
    RilLog("[%d] PS %s", mPhoneId, isPsRestricted ? "restricted by modem for a while" : "resume");

    // update network state again
    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);

    return 0;
}
