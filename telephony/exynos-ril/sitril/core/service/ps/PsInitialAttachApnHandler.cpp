/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsInitialAttachApnHandler.h"
#include "rillog.h"
#include "psservice.h"
#include "PsTimeoutConfig.h"
#include "datacallreqdata.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "psdatabuilder.h"
#include "mcctable.h"
#include <librilutils/textutils.h>

PsInitialAttachApnHandler::PsInitialAttachApnHandler()
{
    m_pPsService = NULL;
    m_AttachApnTypeTable.insert(pair<string, string>("46000", APN_TYPE_IMS));
    m_AttachApnTypeTable.insert(pair<string, string>("46002", APN_TYPE_IMS));
    m_AttachApnTypeTable.insert(pair<string, string>("46007", APN_TYPE_IMS));
}

PsInitialAttachApnHandler::~PsInitialAttachApnHandler()
{
}

void PsInitialAttachApnHandler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

int PsInitialAttachApnHandler::DoRefreshInitialAttachApn(Message *pMsg)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    SetInitialAttachApnRequestData *rildata = NULL;
    if(!GetRilRequestData(pMsg, rildata)) return -1;

    int dataProfileId = DATA_PROFILE_DEFAULT;

    string strSimPlmn = m_pPsService->GetSimOperatorNumeric();
    const char *carrier = strSimPlmn.c_str();
    const char *apn = rildata->GetApn();
    const char *username = rildata->GetUsername();
    const char *password = rildata->GetPassword();
    const char *protocol = rildata->GetProtocol();
    const char *roaming_protocol = rildata->GetRoamingProtocol();
    int authType = rildata->GetAuthType();
    int supportedApnTypesBitmap = rildata->GetSupportedApnTypesBitmap();

    const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo = rildata->GetTdInfo();

    // Check Emergency only InitialAttach Apn Request, if there's no explicit ia type, Do not allow Emergency InitalAttach
    if((supportedApnTypesBitmap & APN_TYPE_BIT_EMERGENCY) && !(supportedApnTypesBitmap & APN_TYPE_BIT_IA)) {
        RilLogW("Emergency InitialAttach is not allowed");
        m_pPsService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    ApnSetting *pNewApnSetting = ApnSetting::NewInstance(carrier, apn, supportedApnTypesBitmap, username, password, protocol, roaming_protocol, authType);
    if (pNewApnSetting == NULL) {
        RilLogV("[%d] Failed to set attach APN info. No APN available", m_pPsService->GetRilSocketId());
        m_pPsService->OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }
    RilLogW("[%d] Create new APN %s", m_pPsService->GetRilSocketId(), pNewApnSetting->ToString().c_str());

    if(rildata->GetModemCognitive() == TRUE) {
        RilLogV("modem cognitive is true");
    } else {
        RilLogV("modem cognitive is false");
    }

    // 1. There is the case which RIL does not detect SIM absent during fast hotswap. So RIL does not send initial attach APN to CP in the below code.
    // 2. CP confirmed that there is no need to check  initial attach APN info in RIL side.
    int ret = RequestSetInitialAttachApn(pNewApnSetting, dataProfileId, tdInfo);
    if (ret < 0) {
        RilLogV("[%d] Failed to set attach APN info.", m_pPsService->GetRilSocketId());
        m_pPsService->OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    if (pNewApnSetting != NULL) {
        delete pNewApnSetting;
        pNewApnSetting = NULL;
    }

    return 0;
}

int PsInitialAttachApnHandler::RequestSetInitialAttachApn(const ApnSetting *pApnSetting, int dataProfileId, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo)
{
    RilLogI("[%s] %s()", m_pPsService->GetServiceName(), __FUNCTION__);

    if (pApnSetting == NULL) {
        RilLogE("[%d] Invalid parameter: ApnSetting is NULL.", m_pPsService->GetRilSocketId());
        return -1;
    }

    ApnSetting *pNewApnSetting = pApnSetting->Clone();
    PdpContext *pPdpContext = ConfigureAttachPdpContext(pNewApnSetting, tdInfo);
    if (pPdpContext == NULL) {
        RilLogE("[%d] No available PDP Context for Attach PDN(PDP)", m_pPsService->GetRilSocketId());
        if (pNewApnSetting != NULL) {
            delete pNewApnSetting;
            pNewApnSetting = NULL;
        }
        return -1;
    }
    pPdpContext->SetDataProfileId(dataProfileId);

    string strSimPlmn = m_pPsService->GetSimOperatorNumeric();
    const char *carrier = strSimPlmn.c_str();
    bool isEsmFlagZero = MccTable::IsEsmFlagZeroOperator(carrier);
    RilLogV("[%d] PDP Context{cid=%d,data profile=%d} isEsmFlagZero=%d", m_pPsService->GetRilSocketId(),
            pPdpContext->GetCID(), pPdpContext->GetDataProfileId(), isEsmFlagZero);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildSetInitialAttachApn(pPdpContext, isEsmFlagZero);
    if (m_pPsService->SendRequest(pModemData, TIMEOUT_DEFAULT_PS, MSG_PS_SET_INITIAL_ATTACH_APN_DONE) < 0) {
        pPdpContext->SetApnSetting(NULL);
        return -1;
    }
    return 0;
}

PdpContext *PsInitialAttachApnHandler::ConfigureAttachPdpContext(ApnSetting *pApnSetting, const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo)
{
    if (pApnSetting == NULL) {
        return NULL;
    }

    // Recover ProfileId from APN_TYPE
    RilLogV("Attach APN Type = %s", pApnSetting->GetTypeString());
    int dataProfileId = DATA_PROFILE_DEFAULT;

    if ((!TextUtils::Equals(pApnSetting->GetTypeString(), APN_TYPE_ALL) && pApnSetting->CanHandleType(APN_TYPE_BIT_IMS)) ||
        (TextUtils::Equals(GetAttachApnType().c_str(), APN_TYPE_IMS) && pApnSetting->CanHandleType(APN_TYPE_BIT_IMS)) ) {
        RilLogV("Attach data profile to IMS");
        dataProfileId = DATA_PROFILE_IMS;
    }

    int attachCid = m_pPsService->GetAttachCid();
    PdpContext *pAttachPdpContext = m_pPsService->GetPdpContext(attachCid);
    if (pAttachPdpContext == NULL) {
        return NULL;
    }

    // TODO need a discussion if old APN is existed and different with new APN
    ApnSetting *pOldApnSetting = pAttachPdpContext->GetApnSetting();
    if (pOldApnSetting != NULL) {

    }

    // set new APN
    pAttachPdpContext->SetApnSetting(pApnSetting);
    pAttachPdpContext->SetDataProfileId(dataProfileId);
    pAttachPdpContext->SetTdSetting(tdInfo);

    return pAttachPdpContext;
}

template<typename T>
bool PsInitialAttachApnHandler::GetRilRequestData(Message *pMsg, T *(&rildata))
{
    if (IsNullMsg(pMsg)) return false;

    rildata = (T *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return false;
    }
    return true;
}

std::string PsInitialAttachApnHandler::GetAttachApnType()
{
    string strSimPlmn = m_pPsService->GetSimOperatorNumeric();
    map<string, string>::iterator iter = m_AttachApnTypeTable.find(strSimPlmn);
    if (iter != m_AttachApnTypeTable.end()) {
        return (iter->second);
    }
    return APN_TYPE_DEFAULT;
}

inline bool PsInitialAttachApnHandler::IsNullMsg(Message *pMsg)
{
    if(pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return  true;
    }
    return false;
}
