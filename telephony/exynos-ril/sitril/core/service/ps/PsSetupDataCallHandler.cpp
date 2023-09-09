/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsSetupDataCallHandler.h"
#include "rillog.h"
#include "psservice.h"
#include "PsTimeoutConfig.h"
#include "datacallreqdata.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "../../../base/modemconfiguration.h"


PsSetupDataCallHandler::PsSetupDataCallHandler()
{
    m_pPsService = NULL;
}

PsSetupDataCallHandler::~PsSetupDataCallHandler()
{
}

void PsSetupDataCallHandler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

int PsSetupDataCallHandler::DoSetupDataCall(Message *pMsg)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    SetupDataCallRequestData *rildata = (SetupDataCallRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    string strSimPlmn = m_pPsService->GetSimOperatorNumeric();
    const char *carrier = strSimPlmn.c_str();
    int rat = rildata->GetRadioTech();
    const char *apn = rildata->GetApn();
    const char *username = rildata->GetUsername();
    const char *password = rildata->GetPassword();
    int authType = rildata->GetAuthType();
    const char *protocol = rildata->GetProtocol();
    const char *roaming_protocol = rildata->GetRoamingProtocol();
    int dataProfileId = rildata->GetDataProfileId();
    int supportedApnTypesBitmap = rildata->GetSupportedApnTypesBitmap();
    bool roamingAllowed = rildata->IsRoamingAllowed();
    int reason = std::stoi(rildata->GetReason());
    int halVer = rildata->GetHalVersion();

    int pduSessionId = rildata->GetPduSessionId();
    const std::optional<RIL_SliceInfo> sliceInfo = rildata->getSliceInfo();
    int optionalSliceInfoFlag = RIL_OPT_INFO_INVALID;
    int optionalSliceSst = SST_NONE;
    int optionalSliceSD = -1;
    int optionalSliceMappedHplmnSst = SST_NONE;
    int optionalSliceMappedHplmnSD = -1;
    if (sliceInfo.has_value()) {
        optionalSliceInfoFlag = RIL_OPT_INFO_VALID;
        optionalSliceSst = sliceInfo->sst;
        optionalSliceSD = sliceInfo->sliceDifferentiator;
        optionalSliceMappedHplmnSst = sliceInfo->mappedHplmnSst;
        optionalSliceMappedHplmnSD = sliceInfo->mappedHplmnSD;
    }

    const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo = rildata->GetTdInfo();

    bool matchedAll = rildata->GetMatchedAllFlag();

    RilLogV("[%s] %s() Opt Info : sst (%d) sd(%d) HplmnSd (%d) HplmnSst (%d)", m_pPsService->GetServiceName(),
            __FUNCTION__, optionalSliceSst, optionalSliceSD, optionalSliceMappedHplmnSD, optionalSliceMappedHplmnSst);

    ApnSetting *pApnSetting = ApnSetting::NewInstance(carrier, apn, supportedApnTypesBitmap, username, password, protocol, roaming_protocol, authType);
    if (pApnSetting == NULL) {
        RilLogW("Cannot create ApnSetting instance");
        m_pPsService->OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, NULL, pMsg);
        return 0;
    }

    PdpContext *pPdpContext;

    // There are 2 fields in the hal in traffic descriptor: osappid and dnn.
    if (tdInfo.has_value() && (!tdInfo->osAppIds.empty() || !tdInfo->dnn.empty())) {
        RilLogV("tdInfo is present, take a path for TD");
        pPdpContext = GetAvailablePdpContextforTD(m_pPsService, pApnSetting, dataProfileId, tdInfo, matchedAll);
    } else {
        RilLogV("no tdInfo, Assumed as 1.5");
        pPdpContext = GetAvailablePdpContext(m_pPsService, pApnSetting, dataProfileId);
    }

    if(pPdpContext == NULL) {
        delete pApnSetting;
    }
    if(!isPdpContextAvailable(m_pPsService, pPdpContext, pMsg)) {
        return 0;
    }

    // Check Radio Power Off/Unavailable Case earlier
    // If CP has no limitation to get SETUP_DATA_CALL in Radio Power off state,
    // We can skip this
    if (GetRilContext()->GetCurrentRadioState() == RADIO_STATE_UNAVAILABLE) {
        RilLogV("[%s] %s() Current Radio State is RADIO_STATE_UNAVAILABLE", m_pPsService->GetServiceName(),  __FUNCTION__);
        m_pPsService->OnSetupDataCallCompletePdpFail(PDP_FAIL_SIGNAL_LOST, pMsg);
        return 0;
    }
    if (GetRilContext()->GetCurrentRadioState() == RADIO_STATE_OFF) {
        RilLogV("[%s] %s() Current Radio State is RADIO_STATE_ROFF", m_pPsService->GetServiceName(),  __FUNCTION__);
        m_pPsService->OnSetupDataCallCompletePdpFail(PDP_FAIL_RADIO_POWER_OFF, pMsg);
        return 0;
    }

    if(isRoamState()) {
        protocol = roaming_protocol;
        RilLogV("isRoamState(%d) roamingAllowed(%d), protocol(%s)", isRoamState(), roamingAllowed, protocol);
    }
    //Now update Protocol type by RoamState
    RilLogV("RoamState:%d, roamingAllowed:%d, updated protocol:%s, original protocol:%s, roaming_protocol:%s",
            isRoamState(), roamingAllowed, protocol, rildata->GetProtocol(), roaming_protocol);
    ApnSetting *apnSetting = pPdpContext->GetApnSetting();
    if (!apnSetting) {
        RilLogE("apnSetting is null, SetupDataCall will fail. pApnSetting:%p, pPdpContext:%p", pApnSetting, pPdpContext);
        m_pPsService->OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, NULL, pMsg);
        return 0;
    }
    apnSetting->UpdateProtocol(protocol);

    ProtocolPsBuilder builder;
    ModemData *pModemData;

    if ( halVer == HAL_VERSION_CODE(1, 4) ) {
        pModemData = builder.BuildSetupDataCall(rat, pPdpContext, m_pPsService->GetRilSocketId(), reason, rildata->GetAddresses());
    } else if ( halVer >= HAL_VERSION_CODE(1, 5) ) {
        string ho_addresses;
        for (int i = 0; i < rildata->mSize; i++) {
            ho_addresses.append(rildata->mLinkAddresses[i].address);
            ho_addresses.append(" ");
        }

        if (halVer == HAL_VERSION_CODE(1, 5)) {
            pModemData = builder.BuildSetupDataCall(rat, pPdpContext, m_pPsService->GetRilSocketId(), reason, ho_addresses.c_str());
            RilLogV("[%s] %d pModemData=%p", __FUNCTION__, __LINE__, pModemData);
        } else {
            bool alwaysOnPdu = false;
            if (halVer == HAL_VERSION_CODE(1, 6)) {
                builder.SetIpcVer(ProtocolPsBuilderInterface::REQ_IPC_VERSION_1);
            } else {
                builder.SetIpcVer(ProtocolPsBuilderInterface::REQ_IPC_VERSION_2);
                alwaysOnPdu = rildata->GetAlwaysOnPdu();
            }
            RilLogV("[%s] %d IPC ver: %d", __FUNCTION__, __LINE__, builder.GetIpcVer());
            pModemData = builder.BuildSetupDataCall(rat, pPdpContext, m_pPsService->GetRilSocketId(), reason, ho_addresses.c_str(),
                                     pduSessionId, optionalSliceInfoFlag, optionalSliceSst, optionalSliceSD, optionalSliceMappedHplmnSst,
                                     optionalSliceMappedHplmnSD, tdInfo, matchedAll, alwaysOnPdu);
            RilLogV("[%s] %d pModemData=%p", __FUNCTION__, __LINE__, pModemData);
            RilLogV("[%s] %s() pduSessionId = %d", m_pPsService->GetServiceName(),  __FUNCTION__, rildata->GetPduSessionId());
        }
    } else {
        pModemData = builder.BuildSetupDataCall(rat, pPdpContext, m_pPsService->GetRilSocketId(), reason, "");
    }

    RilLogV("[%s] %d pModemData=%p", __FUNCTION__, __LINE__, pModemData);
    if (m_pPsService->SendRequest(pModemData, TIMEOUT_SETUP_DATA_CALL, MSG_PS_SETUP_DATA_CALL_DONE, pMsg) < 0) {
        m_pPsService->OnRequestComplete(RIL_E_INVALID_ARGUMENTS, NULL, 0, pMsg);
        return 0;
    }
    rildata->setActivatingPdpCtx(pPdpContext);
    pPdpContext->SetState(PDP_CONTEXT_CONNECTING);

    // embedding handover information, dnsses will be used after SETUP_DATA_CALL_DONE
    if(rildata->GetDnses()) {
        RilLogV("[%s] %s() Store Handover Dnses = %s", m_pPsService->GetServiceName(),  __FUNCTION__, rildata->GetDnses());
        pPdpContext->SetHandoverDnses(string(rildata->GetDnses()));
    } else {
        pPdpContext->SetHandoverDnses(string(""));
    }

#define TEST_FORCE_FAIL_SETUP_DATA (0)
#if (TEST_FORCE_FAIL_SETUP_DATA == 1)
    if (TextUtils::Equals(pApnSetting->GetType(), APN_TYPE_EMERGENCY))
        m_bForceFailAfterSuccess = true;
#endif

    return 0;
}

bool PsSetupDataCallHandler::isRoamState()
{
    int dataNetworkState = m_pPsService->GetDataNetworkState();

    if(dataNetworkState == SIT_NET_REG_STATE_ROAMING ||
       dataNetworkState == SIT_NET_REG_STATE_DENIED_ROAMING)
    {
        return true;
    }
    return false;
}

RilContext *PsSetupDataCallHandler::GetRilContext()
{
    if (m_pPsService == NULL) {
        RilLogW("%s Error no valid PsService instance!!!", __FUNCTION__);
        return NULL;
    }
    return m_pPsService->GetRilContext();
}

// This is reverted code, just keeping for history tracking
bool PsSetupDataCallHandler::isRejectRatForIMS(int rat)
{
    switch(rat) {
    case RADIO_TECH_GPRS:
        return true;
    default:
        return false;
    }
    return false;
}

int PsSetupDataCallHandler::DoSetFastDormancy(void)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    UINT nTargetOperator = 999;
    BYTE bLcdOn=0, bLcdOff=0, bRel8LcdOn=0, bRel8LcdOff=0;

    RilProperty* appProperty = m_pPsService->GetRilApplicationProperty();
    if ( appProperty != NULL ) {
        nTargetOperator = appProperty->GetInt(RIL_APP_TARGET_OPER);
    }

    // 10 = 5 sec
    bLcdOff = 10;  bRel8LcdOff = 10;
    if ( (TARGET_OPER_ATT != nTargetOperator) &&
         (TARGET_OPER_TMO != nTargetOperator) ){
        bLcdOn = 10;  bRel8LcdOn = 10;
    }

    RilLogI("[%s] Target Carrier(%d) support Fast Dormancy with Timer by 5ms*(%d,%d,%d,%d)",
        m_pPsService->GetServiceName(), nTargetOperator, bLcdOn, bLcdOff, bRel8LcdOn, bRel8LcdOff);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildSetFastDormancyInfo(bLcdOn, bLcdOff, bRel8LcdOn, bRel8LcdOff);
    if (m_pPsService->SendRequest(pModemData) < 0) {
        delete pModemData;
        return -1;
    } else {
        delete pModemData;
    }

    return 0;
}

