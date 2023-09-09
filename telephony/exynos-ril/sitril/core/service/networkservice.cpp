/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "networkservice.h"
#include "cscservice.h"
#include "netdatabuilder.h"
#include "operatorinforesolver.h"
#include "operatortable.h"
#include "protocoladapter.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include <librilutils/radio_conv_utils.h>
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include "commondef.h"
#include "util/util.h"
#include "networkutils.h"
#include <unistd.h>

#define MCC_BRAZIL      724

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define CHECK_NULL_MSG(msg, retValue) { if(msg==NULL) { \
                                            RilLogE("[%s] %s() msg is NULL", m_szSvcName, __FUNCTION__); return retValue; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s() msg or RequestData is NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s() msg or ModemData is NULL", m_szSvcName, __FUNCTION__); return -1; } }

NetworkService::NetworkService(RilContext* pRilContext)
    : NetworkServiceBase(pRilContext)
{
    m_lteRrcState = RRC_STATE_IDLE;
    mIsWfcEnabled = false;
}

void NetworkService::OnRadioOffOrNotAvailable()
{
    NetworkServiceBase::OnRadioOffOrNotAvailable();

    // set default LTE RRC state (IDLE)
    UpdateLteRrcState(RRC_IDLE, true);

    mIsWfcEnabled = false;

    // clear a restricted state
    int32_t state = RIL_RESTRICTED_STATE_NONE;
    OnUnsolicitedResponse(RIL_UNSOL_RESTRICTED_STATE_CHANGED, &state, sizeof(state));

    // clear PS restricted state
    RilProperty *property = GetRilContextProperty();
    if (property != nullptr) {
        property->Put(RIL_CONTEXT_NET_PS_RESTRICTED, false);
    }
}

BOOL NetworkService::OnHandleRequest(Message* pMsg)
{
    int ret = -1;
    CHECK_NULL_MSG(pMsg, FALSE);

    switch (pMsg->GetMsgId()) {
    case MSG_NET_SET_NETWORK_SELECTION_MANUAL:
        ret = DoSetNetworkSelectionManual(pMsg);
        break;
    case MSG_NET_SET_NETWORK_SELECTION_MANUAL_WITH_RAT:
        ret = DoSetNetworkSelectionManualWithRat(pMsg);
        break;
    case MSG_NET_QUERY_AVAILABLE_NETWORKS:
        ret = DoQueryAvailableNetwork(pMsg);
        break;
    case MSG_NET_QUERY_BPLMN_SEARCH:
        ret = DoQueryBplmnSearch(pMsg);
        break;
    default:
        return NetworkServiceBase::OnHandleRequest(pMsg);
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL NetworkService::OnHandleSolicitedResponse(Message* pMsg)
{
    CHECK_NULL_MSG(pMsg, FALSE);

    int ret = -1;

    switch (pMsg->GetMsgId()) {
    case MSG_NET_SET_NETWORK_SELECTION_MANUAL_DONE:
        ret = OnSetNetworkSelectionManualDone(pMsg);
        break;
    case MSG_NET_QUERY_AVAILABLE_NETWORKS_DONE:
        ret = OnQueryAvailableNetworkDone(pMsg);
        break;
    case MSG_NET_QUERY_BPLMN_SEARCH_DONE:
        ret = OnQueryBplmnSearchDone(pMsg);
        break;
    default:
        return NetworkServiceBase::OnHandleSolicitedResponse(pMsg);
    } // end switch ~
    return (ret < 0 ? FALSE : TRUE);
}

BOOL NetworkService::OnHandleUnsolicitedResponse(Message* pMsg)
{
    CHECK_NULL_MSG(pMsg, FALSE);

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_NET_RADIO_READY:
        ret = OnRadioReady(pMsg);
        break;
    case MSG_NET_RRC_INFO:
        ret = OnRrcInfo(pMsg);
        break;
    default:
        return NetworkServiceBase::OnHandleUnsolicitedResponse(pMsg);
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL NetworkService::OnHandleRequestTimeout(Message* pMsg)
{
    CHECK_NULL_MSG(pMsg, FALSE);

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_NET_QUERY_AVAILABLE_NETWORKS:
        ret = OnQueryAvailableNetworkTimeout(pMsg);
        break;
    default:
        return NetworkServiceBase::OnHandleRequestTimeout(pMsg);
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

int NetworkService::DoSetNetworkSelectionManual(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

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

    int ran = RIL_RAN_UNKNOWN;
    const char *plmn = NULL;
    StringsRequestData *rildata = (StringsRequestData *)pMsg->GetRequestData();
    plmn = rildata->GetString(0);
    if (rildata->GetStringCount() > 1) {
        if (!TextUtils::IsEmpty(rildata->GetString(1))) {
            ran = strtol(rildata->GetString(1), NULL, 10);
        }
    }

    int rat = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(ran);
    RilLogV("[%s] Select PLMN=%s RAT=%d", m_szSvcName, plmn, rat);

    // For VTS test.
    if (TextUtils::Equals(plmn, "123456")) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS, NULL, 0, pMsg);
        return 0;
    }

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetNetworkSelectionManual(rat, plmn);
    if (SendRequest(pModemData, TIMEOUT_NET_SET_NETWORK_MODE, MSG_NET_SET_NETWORK_SELECTION_MANUAL_DONE, pMsg) < 0) {
        return -1;
    }

    return 0;
}

int NetworkService::DoSetNetworkSelectionManualWithRat(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

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

    int rat = RADIO_TECH_UNKNOWN;
    const char *plmn = NULL;
    // parameter : PLMN or (PLMN + RAT)
    StringsRequestData *rildata = (StringsRequestData *)pMsg->GetRequestData();
    plmn = rildata->GetString(0);

    // For VTS test.
    if (TextUtils::Equals(plmn, "123456")) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS, NULL, 0, pMsg);
        return 0;
    }

    if (rildata->GetStringCount() > 1) {
        if (!TextUtils::IsEmpty(rildata->GetString(1))) {
            rat = strtol(rildata->GetString(1), NULL, 10);
        }
    }

    RilLogV("[%s] Select PLMN=%s RAT=%d", m_szSvcName, plmn, rat);
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetNetworkSelectionManual(rat, plmn);
    if (SendRequest(pModemData, TIMEOUT_NET_SET_NETWORK_MODE, MSG_NET_SET_NETWORK_SELECTION_MANUAL_DONE, pMsg) < 0) {
        return -1;
    }

    return 0;
}


int NetworkService::OnSetNetworkSelectionManualDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, NULL, 0, pMsg);
    }
    else {
        OnRequestComplete(RIL_E_INVALID_STATE, NULL, 0, pMsg);
    }
    return 0;
}

int NetworkService::DoQueryAvailableNetwork(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    // Deprecated by vendor RIL.
    // Using RIL_REQUEST_START_NETWORK_SCAN/RIL_UNSOL_NETWORK_SCAN_RESULT
    // is recommended.
    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, pMsg);
    return 0;
}

int NetworkService::DoQueryBplmnSearch(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_MSG(pMsg, -1);

    UINT nRet = IsOppsiteStackOccupyRF();
    if ( nRet == OCCUPY_RF_STATUS_PLMN_SEARCH )
    {
        RilLogV("[%s] %s() Cancel opposite PLMN searching", m_szSvcName, __FUNCTION__);
        RequestData *pData = RilParser::CreateRawData(RIL_REQUEST_OEM_CANCEL_AVAILABLE_NETWORKS, 0, NULL, 0);

        if (pData != NULL) {
            Message *msg = Message::ObtainMessage(pData, RIL_SERVICE_MISC, MSG_MISC_OEM_CANCEL_AVAILABLE_NETWORKS);
            if ( m_pRilContext != NULL ) {
                RilContext* pTargetRilContext = m_pRilContext->GetOppositeRilContext();
                if ( pTargetRilContext != NULL ) {
                    if ( pTargetRilContext->GetServiceManager()->SendMessage(msg) < 0) {
                        if (msg) {
                            delete msg;
                        }
                    }
                } else {
                    delete msg;
                }
            } else {
                delete msg;
            }
        }

        UINT nWaitCount = 250;
        //RilLogV("[%s] %s(), waiting opposite PLMN searching end", m_szSvcName, __FUNCTION__);
        do
        {
            nRet = IsOppsiteStackOccupyRF();
            WaitForTimeUs(__FUNCTION__, 20U);
        }while ( nRet == OCCUPY_RF_STATUS_PLMN_SEARCH && nWaitCount-- > 0 );
    }

    nRet = IsOppsiteStackOccupyRF();
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

    ProtocolNetworkScanBuilder builder;
    ModemData *pModemData = builder.BuildQueryAvailableNetwork(RIL_RAN_UNKNOWN);
    if (SendRequest(pModemData, TIMEOUT_NET_QUERY_AVAILABLE_NETWORK, MSG_NET_QUERY_BPLMN_SEARCH_DONE, pMsg) < 0) {
        return -1;
    }
    return 0;
}

int NetworkService::OnQueryAvailableNetworkDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    // Deprecated by vendor RIL.
    // Using RIL_REQUEST_START_NETWORK_SCAN/RIL_UNSOL_NETWORK_SCAN_RESULT
    // is recommended.
    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, pMsg);
    return 0;
}

int NetworkService::OnQueryAvailableNetworkTimeout(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_MSG(pMsg, -1);

    // Cancel requested PLMN searching
    ProtocolNetworkScanBuilder builder;
    ModemData *pModemData = builder.BuildCancelQueryAvailableNetwork();
    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) {
            RilLogW("Fail to send Cancel query available networks request ");
        }
        delete pModemData;
    }

    // explicitly error
    OnRequestComplete(RIL_E_GENERIC_FAILURE, NULL, 0, pMsg);
    return 0;
}

int NetworkService::OnQueryBplmnSearchDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    CHECK_NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolNetAvailableNetworkAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        int count = adapter.GetCount();
        string strSimPlmn = GetSimOperatorNumeric();
        const char *simPlmn = strSimPlmn.c_str();
        char simSpn[MAX_FULL_NAME_LEN + 1] = { 0, };
        string spn = GetSimSpn(GetRilSocketId());
        int lenSpn = spn.length();
        if(lenSpn > 0) {
            strncpy(simSpn, spn.c_str(), (lenSpn < MAX_FULL_NAME_LEN)? lenSpn : MAX_FULL_NAME_LEN);
        }

        NetworkDataBplmnListBuilder builder;
        for (int i = 0; i < count; i++) {
            NetworkInfo nwkInfo;
            memset(&nwkInfo, 0, sizeof(nwkInfo));
            if (adapter.GetNetwork(nwkInfo, i, simPlmn, simSpn)) {
                UpdateNetworkNameOfNetworkScanResult(nwkInfo, simPlmn, nwkInfo.plmn, simSpn);
                builder.AddNetworkInfo(nwkInfo);
            }
        } // end for i ~

        const RilData *rildata = builder.Build();
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength(), pMsg);
            delete rildata;
        }
        else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE, NULL, 0, pMsg);
        }
    }
    else {
        OnRequestComplete(errorCode, NULL, 0, pMsg);
    }
    return 0;
}

void NetworkService::OnVoiceRegistrationCustomNotification(int regState, int rejectCause)
{
    if ( regState == DENIED || regState == DENIED_EMERGENCY_ONLY )
    {
        stringstream ss;
        ss << "broadcast -a com.samsung.slsi.action.REJECT_REG -n com.samsung.slsi.telephony.testmode/.TestModeReceiver";
        ss << " --ei rej_cause ";
        ss << rejectCause;
        string intent = ss.str();

        SystemProperty::Set(RIL_NET_REJECT_CAUSE, rejectCause);
        RilLogV("send reject cause display ind, (Reject=%d)", rejectCause);
        OnUnsolicitedResponse(RIL_UNSOL_OEM_AM, intent.c_str(), intent.length());
    }
}

// IOemSamsungslsi@1.1
int NetworkService::OnRrcInfo(Message *pMsg)
{
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    CHECK_NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolNetRrcInfoAdapter adapter(pModemData);
    int rat = adapter.GetRat();
    int state = adapter.GetState();

    if (NetworkUtils::isLte(rat) && state != m_lteRrcState) {
        // update the current LTE RRC state and
        // notify unsolicited response to OEM RIL
        UpdateLteRrcState(state);
    }

    return 0;
}

void NetworkService::UpdateLteRrcState(int state, bool notify/* = true*/)
{
    // update the current LTE RRC state
    if (m_lteRrcState != state) {
        m_lteRrcState = state;
    }

    // notify if needed
    if (notify) {
        OnUnsolicitedResponse(RIL_UNSOL_OEM_LTE_RRC_STATE_CHANGED, &m_lteRrcState, sizeof(m_lteRrcState));
    }
}

void NetworkService::UpdateNetworkNameOfNetworkScanResult(NetworkInfo &nwkInfo,
        const char *simOperatorNumeric, const char *operatorNumeric, const char *simSpn)
{
    std::string networkNumeric = operatorNumeric ? operatorNumeric : "";
    std::string simNumeric = simOperatorNumeric ? simOperatorNumeric : "";
    std::string spn = simSpn ? simSpn : "";

    OperatorInfo info = OperatorInfoResolver().resolve(networkNumeric, simNumeric, spn,
            EonsResolver::createForPhoneId(GetRilSocketId()));
    if (info.valid()) {
        strlcpy(nwkInfo.longPlmn, info.alphaLong.c_str(), MAX_FULL_NAME_LEN);
        strlcpy(nwkInfo.shortPlmn, info.alphaShort.c_str(), MAX_SHORT_NAME_LEN);
    }

    RilLogV("[%s] NeworkInfo from Plmn number {%s/%s/%s/%s RAT=%d}", m_szSvcName,
            nwkInfo.plmn, nwkInfo.longPlmn, nwkInfo.shortPlmn, nwkInfo.status, nwkInfo.rat);
}

BOOL NetworkService::IsPlmnSearching(void)
{
    BOOL ret;
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    ret = IsInTransaction(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS) || IsInTransaction(RIL_REQUEST_QUERY_BPLMN_SEARCH);
    return ret;
}

UINT NetworkService::IsOppsiteStackOccupyRF()
{
    NetworkService *pOppNetworkService = (NetworkService*)GetOppositeService(RIL_SERVICE_NETWORK);
    if ( pOppNetworkService != NULL )
    {
        if ( pOppNetworkService->IsPlmnSearching() == TRUE )
        {
            RilLogV("[%s] %s() : Oppsite RIL context is in PLMN Searching", m_szSvcName, __FUNCTION__);
            return OCCUPY_RF_STATUS_PLMN_SEARCH;
        }
    }

    CscService *pOppCsService = (CscService *)GetOppositeService(RIL_SERVICE_CSC);
    if ( pOppCsService != NULL )
    {
        if ( pOppCsService->IsInCallState() == TRUE )
        {
            RilLogV("[%s] %s() : Oppsite RIL context is in Call state", m_szSvcName, __FUNCTION__);
            return OCCUPY_RF_STATUS_DIAL;
        }
    }
    return 0;
}

UINT NetworkService::IsCurrentStackOccupyRF()
{
    NetworkService* pMyNetworkService = (NetworkService *)GetCurrentService(RIL_SERVICE_NETWORK);
    if ( pMyNetworkService != NULL )
    {
        if ( pMyNetworkService->IsPlmnSearching() == TRUE )
        {
            RilLogV("[%s] %s() : Current RIL context is in PLMN Searching", m_szSvcName, __FUNCTION__);
            return OCCUPY_RF_STATUS_PLMN_SEARCH;
        }
    }

    CscService *pMyCsService = (CscService *)GetCurrentService(RIL_SERVICE_CSC);
    if ( pMyCsService != NULL )
    {
        if ( pMyCsService->IsInCallState() == TRUE )
        {
            RilLogV("[%s] %s() : Oppsite RIL context is in Call state", m_szSvcName, __FUNCTION__);
            return OCCUPY_RF_STATUS_DIAL;
        }
    }
    return 0;
}
