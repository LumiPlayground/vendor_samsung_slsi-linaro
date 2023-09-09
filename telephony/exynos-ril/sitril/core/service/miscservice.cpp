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
 * miscservice.cpp
 *
 *  Created on: 2014. 6. 30.
 *      Author: m.afzal
 *  Modified :
 *    2020. 07. 31 Class Refactoring with using ServiceHandler
 */

#include "carrierconfig/carrierconfigmanager.h"
#include "miscservice.h"
#include <librilutils/systemproperty.h>
#include <telephony/librilutils.h>
#include <librilutils/build.h>
#include "miscdata.h"
#include "miscdatabuilder.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "protocolnetbuilder.h"
#include "reset_util.h"
#include "rillog.h"
#include "rilapplication.h"
#include "ts25table.h"
#include "modemcontrol.h"

#include <string>
#include <sstream>

namespace configV2_0 = android::hardware::radio::config::V2_0;

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s() msg is NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s() msg or RequestData is NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s() msg or ModemData is NULL", m_szSvcName, __FUNCTION__); return -1; } }

#define INVALID_TIMESTAMP (uint64_t)(-1)

MiscService::MiscService(RilContext* pRilContext)
: Service(pRilContext, RIL_SERVICE_MISC)
{
    m_nSubscription = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;

    mCardState = RIL_CARDSTATE_ABSENT;
    mAppState = RIL_APPSTATE_UNKNOWN;

    // By default, report signal strength to OEM.
    // Once getting signal strength ind, don't report anymore
    mDontReportOemSignalStrength = false;

    // if mCurrentSignalStrength is not NULL,
    // return cached signal strength info instead of querying to the modem.
    mCurrentSignalStrength = NULL;
    mLastReceivedTimestamp = INVALID_TIMESTAMP;
    mModemState = MS_OFFLINE;
    mNeedModemStateRecovery = false;
}

MiscService::~MiscService()
{
    if (NULL != mCurrentSignalStrength) {
        delete mCurrentSignalStrength;
        mCurrentSignalStrength = NULL;
    }
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).unregisterMonitoringCallback(this);
    TS25Table::ReleaseInstance();
}

int MiscService::OnCreate(RilContext * /* pRilContext */)
{
    RilLogI("%s::%s()", m_szSvcName, __FUNCTION__);
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).registerMonitoringCallback(this, true);
    return 0;
}

void MiscService::OnStart()
{
    // init TS.25 Table
    if (GetRilSocketId() == RIL_SOCKET_1) {
        RilLogI("%s %s TS25Table::MakeInstance", GetServiceName(), __FUNCTION__);
        TS25Table::MakeInstance();
        CarrierConfigManager::GetInstance();
    }
}

BOOL MiscService::OnHandleRequest(Message* pMsg)
{
    RilLogI("%s::%s()", m_szSvcName, __FUNCTION__);

    INT32 nRet = -1;
    if(NULL == pMsg)
        return FALSE;

    switch (pMsg->GetMsgId())
    {
        case MSG_MISC_SIGNAL_STR:
        {
            nRet = DoSignalStrength(pMsg);
            break;
        }
        case MSG_MISC_OEM_HIDDEN_REQ:
        case MSG_MISC_OEM_FCRASH_MNR_REQ:
        {
            nRet = DoOEMSysDump(pMsg);
            break;
        }
        case MSG_MISC_OEM_SEND_TEST_JSON_DATA:
        {
            nRet = DoRecievedTestJsonData(pMsg);
            break;
        }
        default:
            break;
    }

    if(0 == nRet)
        return TRUE;
    else
        return FALSE;
}
BOOL MiscService::OnHandleSolicitedResponse(Message* pMsg)
{
    RilLogI("%s::%s()", m_szSvcName, __FUNCTION__);
    INT32 nRet = -1;

    if(NULL == pMsg)
        return FALSE;

    //switch (m_pCurReqMsg->GetMsgId()+1)
    switch(pMsg->GetMsgId())
    {
        case MSG_MISC_SIGNAL_STR_DONE:
        {
            nRet = OnSignalStrengthDone(pMsg);
            break;
        }
        default:
            break;
    }

    if(0 == nRet)
        return TRUE;
    else
        return FALSE;

}

BOOL MiscService::OnHandleUnsolicitedResponse(Message* pMsg)
{
    RilLogI("%s::%s()", m_szSvcName, __FUNCTION__);
    if(NULL == pMsg)
        return FALSE;

    switch (pMsg->GetMsgId())
    {
        case MSG_MISC_UNSOL_SIGNAL_STRENGTH:
        {
            OnUnsolSignalStrength(pMsg);
            break;
        }
        default:
            break;
    }

    return TRUE;
}

BOOL MiscService::OnHandleInternalMessage(Message* /* pMsg */)
{
    return TRUE;
}

void MiscService::OnModemStateChanged(int state)
{
    if (GetRilSocketId() == RIL_SOCKET_1) {
        int oldModemState = mModemState;
        mModemState = state;

        if (state == MS_CRASH_EXIT || state == MS_CRASH_RESET) {
            bool alert = (state == MS_CRASH_EXIT);
            OnNotifyModemCrashState(alert);
        }

        bool notifyModemCrashState = (mModemState == MS_CRASH_RESET || mModemState == MS_CRASH_EXIT);
        bool notifyModemRecoveryState = mNeedModemStateRecovery && (mModemState == MS_ONLINE);
        RilLog("oldModemState=%d mModemState=%d notifyModemCrashState=%d notifyModemRecoveryState=%d",
                oldModemState, mModemState, notifyModemCrashState, notifyModemRecoveryState);

        // Two flags can never be true together
        if (notifyModemCrashState || notifyModemRecoveryState) {
            mNeedModemStateRecovery = notifyModemCrashState;
            // 0: online -> crash
            // 1: crash -> online(recovery)
            int state = notifyModemCrashState ? 0 : 1;
            OnUnsolicitedResponse(RIL_UNSOL_OEM_MODEM_STATE_CHANGED, &state, sizeof(state));
        }
    }
}

void MiscService::OnRadioOffOrNotAvailable()
{
    UpdateCurrentSignalStrength(NULL);
}

void MiscService::OnReset()
{
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);
    // When CP reset happen, need notify to f/w.
    const char *pData = "CpReset";
    int lenth = strlen(pData);
    OnUnsolicitedResponse(RIL_UNSOL_MODEM_RESTART, pData, lenth);
}

void MiscService::OnRadioAvailable()
{
    // device information
    SendDeviceInfo();
    SetFeatureInfo();

    if (GetRilSocketId() == RIL_SOCKET_1) {
        // CP debug trace
        SetDebugTraceOffOnBoot();

        // set modems config (single or multi-sim)
        SetModemsConfig();

        // set slot mapping
        SetSlotMapping();

        // SGC for Modem
        SendSGCValue();

        // IMEI SV
        SendSvnInfo();

        SetBtlBufMaxSize();
    }
}


//  handle request


int MiscService::DoSignalStrength(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_MSG(pMsg);

    if (!IsQueryingSignalStrengthNeed()) {
        RilLogV("[%d] Return cached signal strength", GetRilSocketId());
        PrintCurrentSignalStrength();
        OnRequestComplete(RIL_E_SUCCESS, mCurrentSignalStrength, sizeof(RIL_SignalStrength_V1_4));
        return 0;
    }

    RilLogV("[%d] Query next signal strength", GetRilSocketId());
    ProtocolDeviceInfoBuilder builder;
    ModemData *pModemData = builder.GetSignalStrength();
    if (SendRequest(pModemData, TIMEOUT_MISC_DEFAULT, MSG_MISC_SIGNAL_STR_DONE) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}


void MiscService::OnNotifyModemCrashState(bool alert)
{
    int mode = SystemProperty::GetInt(CRASH_MODE_SYS_PROP, CRASH_MODE_DEFAULT);
    bool show = alert && (mode == CRASH_MODE_DUMP_PANIC);

    //char logDump[] = "startservice -n com.samsung.slsi.sysdebugmode/com.samsung.slsi.logdump.LogDumpService";
    //OnUnsolicitedResponse(RIL_UNSOL_OEM_AM, (unsigned char *)&logDump, strlen(logDump));

    if (show) {
        // Red screen
        char redScreen[] = "start -n com.samsung.slsi.sysdebugmode/com.samsung.slsi.logdump.ReceiverActivity";
        OnUnsolicitedResponse(RIL_UNSOL_OEM_AM, (unsigned char *)&redScreen, strlen(redScreen));
    }
}

int MiscService::OnSignalStrengthDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    RilProperty *prop = GetRilApplicationProperty();
    int ssVer = prop->GetInt(STRING_SIGNAL_STRENGTH, SIGNAL_STRENGTH_PROTOCOL_DEFAULT);
    ProtocolSignalStrengthAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RIL_SignalStrength_V1_6 currentSignalStrength = adapter.GetSignalStrength(ssVer);
        SignalStrengthBuilder buidler(GetCurrentRequestDataHalVersion(RilApplication::RIL_HalVersionCode));
        const RilData *rildata = buidler.Build(currentSignalStrength);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

//handle unsolicited response
void MiscService::PrintCurrentSignalStrength()
{
    if (mCurrentSignalStrength != NULL) {
        RilLogV("[%d] RIL_SignalStrength_V1_4(%llu) " \
                "GSM[%d %d %d] CDMA[%d %d] EVDO[%d %d %d] " \
                "LTE[%d %d %d %d %d %d] TD-SCDMA[%u %u %u] WCDMA[%d %d %u %u] " \
                "NR[%d %d %d %d %d %d]",
                GetRilSocketId(), (unsigned long long)mLastReceivedTimestamp,
                mCurrentSignalStrength->GSM_SignalStrength.signalStrength,
                mCurrentSignalStrength->GSM_SignalStrength.bitErrorRate,
                mCurrentSignalStrength->GSM_SignalStrength.timingAdvance,
                mCurrentSignalStrength->CDMA_SignalStrength.dbm,
                mCurrentSignalStrength->CDMA_SignalStrength.ecio,
                mCurrentSignalStrength->EVDO_SignalStrength.dbm,
                mCurrentSignalStrength->EVDO_SignalStrength.ecio,
                mCurrentSignalStrength->EVDO_SignalStrength.signalNoiseRatio,
                mCurrentSignalStrength->LTE_SignalStrength.signalStrength,
                mCurrentSignalStrength->LTE_SignalStrength.rsrp,
                mCurrentSignalStrength->LTE_SignalStrength.rsrq,
                mCurrentSignalStrength->LTE_SignalStrength.rssnr,
                mCurrentSignalStrength->LTE_SignalStrength.cqi,
                mCurrentSignalStrength->LTE_SignalStrength.timingAdvance,
                mCurrentSignalStrength->TD_SCDMA_SignalStrength.signalStrength,
                mCurrentSignalStrength->TD_SCDMA_SignalStrength.bitErrorRate,
                mCurrentSignalStrength->TD_SCDMA_SignalStrength.rscp,
                mCurrentSignalStrength->WCDMA_SignalStrength.signalStrength,
                mCurrentSignalStrength->WCDMA_SignalStrength.bitErrorRate,
                mCurrentSignalStrength->WCDMA_SignalStrength.rscp,
                mCurrentSignalStrength->WCDMA_SignalStrength.ecno,
                mCurrentSignalStrength->NR_SignalStrength.ssRsrp,
                mCurrentSignalStrength->NR_SignalStrength.ssRsrq,
                mCurrentSignalStrength->NR_SignalStrength.ssSinr,
                mCurrentSignalStrength->NR_SignalStrength.csiRsrp,
                mCurrentSignalStrength->NR_SignalStrength.csiRsrq,
                mCurrentSignalStrength->NR_SignalStrength.csiSinr );
    }
    else {
        RilLogV("No cached signal strength info");
    }
}

void MiscService::UpdateCurrentSignalStrength(RIL_SignalStrength_V1_4 *signalStrength)
{
    if (mCurrentSignalStrength != NULL) {
        delete mCurrentSignalStrength;
        mCurrentSignalStrength = NULL;
        mLastReceivedTimestamp = INVALID_TIMESTAMP;
    }

    if (signalStrength != NULL) {
        mCurrentSignalStrength = new RIL_SignalStrength_V1_4;
        if (mCurrentSignalStrength != NULL) {
            memcpy(mCurrentSignalStrength, signalStrength, sizeof(RIL_SignalStrength_V1_4));
            mLastReceivedTimestamp = ril_nano_time();
            PrintCurrentSignalStrength();
        }
        else {
            RilLogW("[%s] memory alloc error", __FUNCTION__);
        }
    }
}

bool MiscService::IsQueryingSignalStrengthNeed()
{
    // allow querying the current signal strength if
    //   - never received unsolicited signal strength at all
    //   - invalid last received time stamp
    // mCurrentSignalStrength and mLastReceivedTimestamp will be cleared
    // when radio power goes to off or unavailable state.
    if (mCurrentSignalStrength != NULL && mLastReceivedTimestamp != INVALID_TIMESTAMP) {
        return false;
    }

    return true;
}

int MiscService::OnUnsolSignalStrength(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();

    RilProperty *prop = GetRilApplicationProperty();
    int ssVer = prop->GetInt(STRING_SIGNAL_STRENGTH, SIGNAL_STRENGTH_PROTOCOL_DEFAULT);

    ProtocolSignalStrengthIndAdapter adapter(pModemData);
    RIL_SignalStrength_V1_6 currentSignalStrength = adapter.GetSignalStrength(ssVer);
    int halVer = RilApplication::RIL_HalVersionCode;
    SignalStrengthBuilder buidler(halVer);
    const RilData *rildata = buidler.Build(currentSignalStrength);
    if (rildata != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_SIGNAL_STRENGTH, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    // Do not report unsolicited OEM signal strength anymore
    // when received a solicited response
    mDontReportOemSignalStrength = true;

    return 0;
}

void MiscService::RequestDeviceReset(int reset_type)
{
    stringstream ss;
    ss << "broadcast -a com.samsung.slsi.action.POWER -n com.samsung.slsi.telephony.testmode/.TestModeReceiver";
    ss << " --ei reset_type ";
    ss << reset_type;
    string intent = ss.str();

    SystemProperty::Set(VENDOR_RIL_RESET_TYPE, reset_type);
    RilLogV("send device reset request, (reset type=%d)", reset_type);
    OnUnsolicitedResponse(RIL_UNSOL_OEM_AM, intent.c_str(), intent.length());
}

bool MiscService::IsPossibleToPassInRadioOffState(int request_id)
{
    switch (request_id) {
        case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
        case RIL_REQUEST_BASEBAND_VERSION:
        case RIL_REQUEST_DEVICE_IDENTITY:
        case RIL_REQUEST_GET_IMEI:
        case RIL_REQUEST_GET_IMEISV:
        case RIL_REQUEST_OEM_MODEM_DUMP:
        case RIL_REQUEST_OEM_SET_ENG_MODE:
        case RIL_REQUEST_OEM_SET_SCR_LINE:
        case RIL_REQUEST_OEM_SET_DEBUG_TRACE:
        case RIL_REQUEST_NV_READ_ITEM:
        case RIL_REQUEST_NV_WRITE_ITEM:
        case RIL_REQUEST_NV_RESET_CONFIG:
        case RIL_REQUEST_GET_HARDWARE_CONFIG:
        case RIL_REQUEST_OEM_SET_ENG_STRING_INPUT:
        case RIL_REQUEST_OEM_GET_MSL_CODE:
        case RIL_REQUEST_OEM_SET_PIN_CONTROL:
        case RIL_REQUEST_OEM_SEND_SGC:
        case RIL_REQUEST_OEM_GET_MANUAL_BAND_MODE:
        case RIL_REQUEST_OEM_SET_MANUAL_BAND_MODE:
        case RIL_REQUEST_OEM_GET_RF_DESENSE_MODE:
        case RIL_REQUEST_OEM_SET_RF_DESENSE_MODE:
        case RIL_REQUEST_OEM_STORE_ADB_SERIAL_NUMBER:
        case RIL_REQUEST_OEM_READ_ADB_SERIAL_NUMBER:
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
        case RIL_REQUEST_SET_VOICE_OPERATION:
        case RIL_REQUEST_OEM_SET_VOICE_OPERATION:
        case RIL_REQUEST_OEM_GET_VOICE_OPERATION:
        case RIL_REQUEST_OEM_SET_PREFERRED_CALL_CAPABILITY:
        case RIL_REQUEST_OEM_GET_PREFERRED_CALL_CAPABILITY:
        case RIL_REQUEST_SCREEN_STATE:
        case RIL_REQUEST_SET_DEVICE_INFO:
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
        case RIL_REQUEST_SEND_DEVICE_STATE:
        case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
        case RIL_REQUEST_OEM_AIMS_ADD_PDN_INFO:
        case RIL_REQUEST_OEM_AIMS_DEL_PDN_INFO:
        case RIL_REQUEST_OEM_AIMS_STACK_START_REQ:
        case RIL_REQUEST_OEM_AIMS_STACK_STOP_REQ:
        case RIL_REQUEST_OEM_AIMS_HIDDEN_MENU:
        case RIL_REQUEST_OEM_SCAN_RSSI:
        case RIL_REQUEST_OEM_FORWARDING_AT_COMMAND:
        case RIL_REQUEST_OEM_AIMS_SEND_SMS_ACK:
        case RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_SMS:
        case RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_CDMA_SMS:
        case RIL_REQUEST_OEM_GET_PLMN_NAME_FROM_SE13TABLE:
        case RIL_REQUEST_OEM_TS25TABLE_DUMP:
        case RIL_REQUEST_GET_PHONE_CAPABILITY:
        case RIL_REQUEST_SET_MODEMS_CONFIG:
        case RIL_REQUEST_GET_MODEMS_CONFIG:
        case RIL_REQUEST_GET_MODEM_STATUS:
        case RIL_REQUEST_OEM_MODEM_INFO:
        case RIL_REQUEST_OEM_MODEM_RESET:
        case RIL_REQUEST_OEM_SWITCH_MODEM_FUNCTION:
        case RIL_REQUEST_OEM_REQ_SET_PDCP_DISCARD_TIMER:
        case RIL_REQUEST_OEM_SET_SELFLOG:
        case RIL_REQUEST_OEM_GET_SELFLOG_STATUS:
        case RIL_REQUEST_SET_ACTIVATE_VSIM:
        case RIL_REQUEST_OEM_GET_CQI_INFO:
        case RIL_REQUEST_OEM_SET_SAR_SETTING:
        case RIL_REQUEST_OEM_SET_IMS_TEST_MODE:
        case RIL_REQUEST_OEM_SET_GMO_SWITCH:
        case RIL_REQUEST_OEM_SET_TCS_FCI:
        case RIL_REQUEST_OEM_GET_TCS_FCI:
        case RIL_REQUEST_OEM_SET_CA_BANDWIDTH_FILTER:
        case RIL_REQUEST_OEM_SET_ELEVATOR_SENSOR:
        case RIL_REQUEST_SET_LOCATION_UPDATES:
        case RIL_REQUEST_SET_TTY_MODE:
        case RIL_REQUEST_QUERY_TTY_MODE:
        case RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
        case RIL_REQUEST_OEM_SET_SELFLOG_PROFILE:
        case RIL_REQUEST_OEM_SET_FORBID_LTE_CELL:
        case RIL_REQUEST_OEM_AIMS_SET_HIDDEN_MENU_ITEM:
        case RIL_REQUEST_OEM_AIMS_GET_HIDDEN_MENU_ITEM:
        case RIL_REQUEST_OEM_SET_AP_SYSTEM_TIME:
        case RIL_REQUEST_OEM_GET_MODEM_ACTIVITY_INFO:
        case RIL_REQUEST_OEM_SET_SVN:
        case RIL_REQUEST_OEM_SET_FORBID_NR_CELL:
        case RIL_REQUEST_OEM_GET_BINARY_VERSION_INFO:
        case RIL_REQUEST_OEM_SET_FEATURE_INFO:
        case RIL_REQUEST_OEM_MISC_SET_TELEPHONY_STATUS_LOGGING:
        case RIL_REQUEST_OEN_MISC_TEST_UNSOL:
        case RIL_REQUEST_OEM_SEND_TEST_JSON_DATA:
        case RIL_REQUEST_OEM_SET_CP_CARRIER_CONFIG:
        case RIL_REQUEST_OEM_SET_SRS_PWR_LIMIT:
        case RIL_REQUEST_OEM_SET_TRACE_DUMP_CFG:
        case RIL_REQUEST_OEM_GET_TRACE_DUMP_CFG:
        case RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA:
        case RIL_REQUEST_OEM_SET_BTL_BUFMAX_SIZE:
        case RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
        case RIL_REQUEST_OEM_SET_NR_DISABLE_BAND:
            break;
        default:
            return false;
    }
    return true;

}

bool MiscService::IsPossibleToPassInRadioUnavailableState(int request_id)
{
    switch(request_id) {
    case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
    case RIL_REQUEST_SET_DEVICE_INFO:
    case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
    case RIL_REQUEST_SEND_DEVICE_STATE:
    case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER:
    case RIL_REQUEST_OEM_GET_PLMN_NAME_FROM_SE13TABLE:
    case RIL_REQUEST_OEM_TS25TABLE_DUMP:
    case RIL_REQUEST_OEM_SEND_TEST_JSON_DATA:
    case RIL_REQUEST_OEM_SET_CP_CARRIER_CONFIG:
    case RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
    case RIL_REQUEST_OEM_SET_NR_DISABLE_BAND:
        return true;

    }
    return false;
}

void MiscService::InitPropertyForGidValue(int cardState) {
    if (cardState != RIL_CARDSTATE_PRESENT) {
        RilProperty *pProperty = GetRilContextProperty();
        if (pProperty != NULL) {
            pProperty->Put(RIL_CONTEXT_SIM_GID1, "");
        }
    }
}

void MiscService::OnSimStatusChanged(int cardState, int appState)
{
    mCardState = cardState;
    mAppState = appState;

    InitPropertyForGidValue(cardState);

    RilLogV("[%s] Card state: %d, App state: %d", __FUNCTION__, cardState, appState);
}

void MiscService::SetDebugTraceOffOnBoot()
{
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);
    // 0: debug on, 1: debug off, 2:notset
    int traceOffOption = SystemProperty::GetInt(VENDOR_CP_DEBUG_OFF_ON_BOOT, 99);

    // if user mode & not set any value
    // if user select to debug off explicitly
    if ( (Build::IS_USER && traceOffOption == 99)
        || traceOffOption == 1 ) {
        if (GetRilContext() != NULL) {
            // Set CP Debug Trace Off
            BYTE debugOnOff[1] = {0};   //0:disable, 1:enable
            GetRilContext()->OnRequest(RIL_REQUEST_OEM_SET_DEBUG_TRACE, debugOnOff, sizeof(char), 0);
        }
    }
}

void MiscService::SetModemsConfig()
{
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);
    // numOfLiveModems
    int numOfLiveModems = RilApplication::IsMultiSimEnabled() ? 2 : 1;
    if (GetRilContext() != NULL) {
        GetRilContext()->OnRequest(RIL_REQUEST_SET_MODEMS_CONFIG, &numOfLiveModems, sizeof(numOfLiveModems), 0);
    }
}

void MiscService::SetSlotMapping()
{
    if (GetRilContext() == NULL) {
        return;
    }
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);

    int multiSimConfig = RilApplication::IsMultiSimEnabled() ? 2 : 1;
    string slotMapConfigInput = SystemProperty::Get("persist.radio.slotmap.config");
    if (slotMapConfigInput.empty()) {
        RilLogV("[%s] %s persist.radio.slotmap.config is empty", GetServiceName(), __FUNCTION__);
        return;
    }

    slotMapConfigInput.erase(remove(slotMapConfigInput.begin(), slotMapConfigInput.end(), ' '), slotMapConfigInput.end());
    slotMapConfigInput.erase(remove(slotMapConfigInput.begin(), slotMapConfigInput.end(), ','), slotMapConfigInput.end());

    configV2_0::RIL_SlotPortMappingReq slotPortMappingReq = {};
    slotPortMappingReq.num_slots = multiSimConfig;
    if (slotMapConfigInput.length() == 2 && multiSimConfig == 1) {
        slotPortMappingReq.slotPortMap[0].physicalSlotId = slotMapConfigInput[0] - '0';
        slotPortMappingReq.slotPortMap[0].portId = slotMapConfigInput[1] - '0';
    } else if ((slotMapConfigInput.length() == 4 && multiSimConfig == 2)) {
        slotPortMappingReq.slotPortMap[0].physicalSlotId = slotMapConfigInput[0] - '0';
        slotPortMappingReq.slotPortMap[0].portId = slotMapConfigInput[1] - '0';
        slotPortMappingReq.slotPortMap[1].physicalSlotId = slotMapConfigInput[2] - '0';
        slotPortMappingReq.slotPortMap[1].portId = slotMapConfigInput[3] - '0';
    } else {
        RilLogV("[%s] %s persist.radio.slotmap.config invalid input", GetServiceName(), __FUNCTION__);
        SystemProperty::Set("persist.radio.slotmap.config", "");
        return;
    }

    int request = ENCODE_REQUEST(RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, HAL_VERSION_CODE(2, 0));
    GetRilContext()->OnRequest(request, &slotPortMappingReq, sizeof(slotPortMappingReq), NULL);
    SystemProperty::Set("persist.radio.slotmap.config", "");
    return;
}

void MiscService::SendDeviceInfo()
{
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);
    string model = SystemProperty::Get(RO_PRODUCT_MODEL, "");
    string swVer = SystemProperty::Get(RO_BUILD_ID, "");
    string productName = SystemProperty::Get(RO_PRODUCT_NAME, "");
    string apOsVersion = SystemProperty::Get(RO_BUILD_VERSION_RELEASE, "");
    RilLogV("Model:%s, SW Ver:%s Product Name:%s OS Ver:%s", model.c_str(), swVer.c_str(), productName.c_str(), apOsVersion.c_str());
    if (GetRilContext() != NULL) {
        const char *ptrStrings[] = { model.c_str(), swVer.c_str(), productName.c_str(), apOsVersion.c_str() };
        GetRilContext()->OnRequest(RIL_REQUEST_SET_DEVICE_INFO, ptrStrings, sizeof(ptrStrings), 0);
    }
}

void MiscService::SendSGCValue()
{
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);
    //Read SGC Value
    RilProperty* property = GetRilApplicationProperty();
    int target_op = property->GetInt(RIL_APP_TARGET_OPER);
    RilLogV("[%s] Check target operator=%d", __FUNCTION__, target_op);
    if (GetRilContext() != NULL) {
        GetRilContext()->OnRequest(RIL_REQUEST_OEM_SEND_SGC, &target_op, sizeof(int), 0);
    }

    return;
}

void MiscService::SendSvnInfo()
{
    RilLogV("[%s] %s", GetServiceName(), __FUNCTION__);

    string imeiSvn = SystemProperty::Get(VENDOR_IMEI_SVN);
    RilLogV("[%s] %s() SV number(%s)", m_szSvcName, __FUNCTION__, imeiSvn.c_str());

    if ((imeiSvn.length() > 0) && (imeiSvn.length() <= 2)) {
        if (imeiSvn.length() == 1) {
            imeiSvn = "0" + imeiSvn;
        }

        for (int i = 0; i < 2; i++) {
            if (imeiSvn[i] < 0x30 || imeiSvn[i] > 0x39) {
                RilLogW("[%s] %s() Wrong character", m_szSvcName, __FUNCTION__);
                return;
            }
        }

        ProtocolNetworkBuilder builder;
        ModemData *pModemData = builder.BuildSvNumber(imeiSvn.c_str());
        if (pModemData != NULL) {
            if (SendRequest(pModemData) < 0) {
                RilLogW("Fail to send SVN");
            }
            delete pModemData;
        }
    } else {
        RilLogW("[%s] %s() SV number empty",m_szSvcName, __FUNCTION__);
    }
}

void MiscService::SetBtlBufMaxSize() {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);

    int size = SystemProperty::GetInt(RIL_VENDOR_BTL_BUFMAX_SIZE, 32);
    OnRequest(RIL_REQUEST_OEM_SET_BTL_BUFMAX_SIZE, &size, sizeof(size));
}

void MiscService::SetFeatureInfo()
{
    if (GetRilContext() != NULL) {
        GetRilContext()->OnRequest(RIL_REQUEST_OEM_SET_FEATURE_INFO, 0, 0, 0);
    }
}

int MiscService::DoOEMSysDump(Message *pMsg) {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    CHECK_NULL_MSG(pMsg);

    int ret = -1;
    if (pMsg->GetMsgId() == MSG_MISC_OEM_FCRASH_MNR_REQ) {
        RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
        int rcmId = 0xFFFF;
        if (rildata == NULL) { RilLogE("[%s] %s() rildata is NULL", GetServiceName(), __FUNCTION__);
        } else {
            int *miscData = (int *)rildata->GetRawData();
            if (miscData != NULL) rcmId = miscData[0];
        }
        ret = DoForceCpCrash(LOG_DUMP_CAUSE_CP_CRASH_MNR, rcmId);
    } else {
        ret = DoForceCpCrash();
    }

    OnRequestComplete((ret == 0) ? RIL_E_SUCCESS : RIL_E_INTERNAL_ERR, NULL, 0, pMsg);
    return 0;
}

// do force crash
int MiscService::DoForceCpCrash(LogDumpCause crash_reason, int _info) {
    char reason[CP_RESET_INFO_SIZE] = {0, };

    if (crash_reason == LOG_DUMP_CAUSE_CP_CRASH_APP) {
        strncpy(reason, "APP\n", CP_RESET_INFO_SIZE);
    } else if (crash_reason == LOG_DUMP_CAUSE_CP_CRASH_MNR) {
        snprintf(reason, CP_RESET_INFO_SIZE-1, "SIT timeout 0x%04x\n", _info);
    } else {
        strncpy(reason, "Unknown\n", CP_RESET_INFO_SIZE);
    }
    reason[CP_RESET_INFO_SIZE-1] = 0;

    if (ModemControl::CrashModem(reason) < 0) {
        RilLogE("Force CP Crash Failed.");
        return -1;
    }
    return 0;
}

int MiscService::DoRecievedTestJsonData(Message *pMsg) {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();

    if(rildata == NULL) {
        OnRequestComplete(RIL_E_SYSTEM_ERR);
        return -1;
    }

    OnRequestComplete(RIL_E_SUCCESS, (void *) rildata->GetRawData(), rildata->GetSize());
    return 0;
}

void MiscService::dump(std::stringstream& ss) {
    ss << GetServiceName() << ":{";
    ss << "SGC=" << GetRilApplicationProperty()->GetInt(RIL_APP_TARGET_OPER);
    ss << "}";
}
