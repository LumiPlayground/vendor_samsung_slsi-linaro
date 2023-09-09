/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "psservice.h"
#include <librilutils/systemproperty.h>
#include "rillog.h"
#include "datacallreqdata.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "protocoladapter.h"
#include "psdatabuilder.h"
#include "rilapplication.h"
#include "PsSetupDataCallHandler.h"
#include "PsTestHandler.h"
#include "PsUtil.h"
#include "PsInitialAttachApnHandler.h"
#include "../../base/modemconfiguration.h"
#include "ps/PsSlicingConfigHandler.h"

#include "./pslog.h"
#include <librilutils/textutils.h>

PsService::PsService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_PS) {
}

template <typename T>
void PsService::cleanUpVar(T &v) {
    if (v != NULL) {
        delete v;
        v = NULL;
    }
}

PsService::~PsService()
{
    cleanUpVar(m_AttachPdpContext);

    RilLogV("Clean up, Max PDP Context size=%d", m_nPdpContextSize);
    for (int i = 0; i < m_nPdpContextSize; i++) {
        cleanUpVar(m_PdpContext[i]);
    } // end for i ~

    cleanUpVar(m_pDeactivatingPdpContext);
    cleanUpVar(m_pSetupDataCallHandler);
    cleanUpVar(m_pTestHandler);
    cleanUpVar(m_pInitialAttachApnHandler);
    cleanUpVar(m_pPsSlicingConfigHandler);

    clearDataProfileTable();

    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).unregisterMonitoringCallback(this);
}

template <typename T>
void PsService::InitHandler(T *(&h)) {
    h = new T();
    if (h != NULL) {
        h->Init(this);
    }
}

int PsService::OnCreate(RilContext * /* pRilContext */) {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);

    if(InitPdpContext() != TRUE) {
        RilLogE("[%s] %s Fail to InitPdpContext", m_szSvcName, __FUNCTION__);
        return -1;
    }

    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);

    InitHandler<PsSetupDataCallHandler>(m_pSetupDataCallHandler);
    InitHandler<PsTestHandler>(m_pTestHandler);
    InitHandler<PsInitialAttachApnHandler>(m_pInitialAttachApnHandler);
    InitHandler<PsSlicingConfigHandler>(m_pPsSlicingConfigHandler);

    // register
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).registerMonitoringCallback(this, true);

    return 0;
}

void PsService::OnDestroy() {
    ResetPdpContext();
}

BOOL PsService::OnHandleRequest(Message* pMsg) {
    int ret = -1;
    if (pMsg == NULL) return FALSE;

    switch (pMsg->GetMsgId()) {
    case MSG_PS_SETUP_DATA_CALL:
        ret = m_pSetupDataCallHandler->DoSetupDataCall(pMsg);
        break;
    case MSG_PS_SET_INITIAL_ATTACH_APN:
    case MSG_PS_REFRESH_INITIAL_ATTACH_APN:
        ret = m_pInitialAttachApnHandler->DoRefreshInitialAttachApn(pMsg);
        break;
    case MSG_PS_SLICING_CONFIG:
        ret = m_pPsSlicingConfigHandler->DoSlicingConfig(pMsg);
        break;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL PsService::OnHandleSolicitedResponse(Message* pMsg) {
    if (pMsg == NULL) return FALSE;

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_PS_SETUP_DATA_CALL_DONE:
        ret = OnSetupDataCallDone<ProtocolPsSetupDataCallAdapter>(pMsg);
        break;
    case MSG_PS_SET_INITIAL_ATTACH_APN_DONE:
        ret = OnSimpleRequestDone(pMsg);
        break;
    case MSG_PS_SLICING_CONFIG_DONE:
        ret = m_pPsSlicingConfigHandler->OnSlicingConfigDone(pMsg);
        break;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL PsService::OnHandleRequestTimeout(Message* pMsg) {
    if (pMsg == NULL) return FALSE;

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_PS_SETUP_DATA_CALL:
        ret = OnSetupDataCallTimeout(pMsg);
        break;
    } // end switch ~
    return (ret < 0 ? FALSE : TRUE);
}

void PsService::OnRadioStateChanged(int radioState) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    switch (radioState) {
    case RADIO_STATE_OFF:
    case RADIO_STATE_UNAVAILABLE:
        m_pDeactivatingPdpContext = NULL;
        m_bIsAttachDone = false;
        ResetPdpContext(true);
        break;

    } // end switch ~
}

void PsService::OnSimStatusChanged(int cardState, int /* appState */) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if ( cardState == RIL_CARDSTATE_ABSENT ) {
        if ( m_pDeactivatingPdpContext != NULL ) {
            RilLogW("In Pdp deactivating state : notify deact. success anyway");
            OnDeactDataCallComplete(m_pDeactivatingPdpContext);
            m_pDeactivatingPdpContext = NULL;
        }
        m_bIsAttachDone = false;
        ResetPdpContext();
        OnNotifyDataCallList();
    }
}

void PsService::OnImsiUpdated(const char *imsi) {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    if (TextUtils::IsEmpty(imsi) || !(strlen(imsi) >= 5)) {
        RilLogE("Invalid IMSI value. Please check it");
        return ;
    }

    string strOldImsi = m_imsi;
    string strNewImsi = imsi;
    memset(m_imsi, 0, sizeof(m_imsi));
    strncpy(m_imsi, imsi, MAX_IMSI_LEN);

    if (!TextUtils::IsEmpty(strOldImsi) && !TextUtils::Equals(strOldImsi, strNewImsi)) {
        RilLog("[%s] IMSI switched : %s -> %s", GetServiceName(), strOldImsi.c_str(), strNewImsi.c_str());
        PdpContext *pPdpContext = RetrieveAttachPdpContext();
        if (pPdpContext != NULL) {
            pPdpContext->SetApnSetting(NULL);
            pPdpContext->SetTdSetting(std::nullopt);
        }
    }

}

BOOL PsService::InitPdpContext() {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);
    const char *ifprefix = NULL;
    int istart = 0;
    m_nPdpContextSize = sizeof(m_PdpContext) / sizeof(m_PdpContext[0]);
    const RilContextParam *param = m_pRilContext->GetRilContextParam();
    if (param != NULL) {
        ifprefix = param->ifprefix;
        istart = param->ifstart;
        m_nAttachCid = istart + 1;
        if ((unsigned int)m_nPdpContextSize > param->ifmaxsize) m_nPdpContextSize = param->ifmaxsize;
    }

    RilLogV("Prepare Attach PdpContext");
    m_AttachPdpContext = new PdpContext(m_nAttachCid, ifprefix, istart);
    if (m_AttachPdpContext != NULL)  m_AttachPdpContext->Init();

    RilLogV("Max PDP Context size=%d", m_nPdpContextSize);
    for (int i = 0; i < m_nPdpContextSize; i++, istart++) {
        int cid = istart + 1;
        m_PdpContext[i] = new PdpContext(cid, ifprefix, istart);
        if (m_PdpContext[i] != NULL) m_PdpContext[i]->Init();
    } // end for i ~

    return TRUE;
}

BOOL PsService::ResetPdpContext(bool bKeepAttachApn) {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);
    for (int i = 0; i < m_nPdpContextSize; i++) {
        PdpContext *pPdpContext = GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            // reset PDP Context and bring down interfaces except for EIMS OnSimStatusChanged
            if((pPdpContext->GetApnSetting() != NULL) &&
               ((pPdpContext->GetApnSetting()->CanHandleType(APN_TYPE_BIT_EMERGENCY)) ||
                (pPdpContext->GetApnSetting()->CanHandleType(APN_TYPE_BIT_IMS)) ) &&
               (bKeepAttachApn == false) && (m_bIsAttachDone == false)) {
                RilLogV("Skip ResetPdpContext EIMS");
            } else if(pPdpContext->GetState() == PDP_CONTEXT_CONNECTING) {
                // Check Connecting status of PDP ctx
                RilLogV("Skip ResetPdpContext for 'CONNECTING' case (cid:%d)", pPdpContext->GetCID());
            } else {
                pPdpContext->OnDeactivated();
            }
            // delete old APN setting information
            if ( bKeepAttachApn == false || pPdpContext->GetCID() != GetAttachCid() ) {
                pPdpContext->SetApnSetting(NULL);
                pPdpContext->SetTdSetting(std::nullopt);
            }
        }
    }
    return TRUE;
}

PdpContext *PsService::GetPdpContext(int cid) {
    for (int i = 0; i < m_nPdpContextSize; i++) {
        PdpContext *pPdpContext = m_PdpContext[i];
        if (pPdpContext != NULL && pPdpContext->GetCID() == cid) {
            return pPdpContext;
        }
    } // end for i ~
    RilLogW("[%s] %s() Not found PDPContext for cid = %d", m_szSvcName, __FUNCTION__, cid);

    return NULL;
}

PdpContext *PsService::GetPdpContextByIndex(int index) {
    if (index >= 0 && index < m_nPdpContextSize) {
        return m_PdpContext[index];
    }

    RilLogW("[%s] %s() Out of range : index=%d", m_szSvcName, __FUNCTION__, index);
    return NULL;
}

PdpContext *PsService::RetrieveAttachPdpContext() {
    return GetPdpContext(GetAttachCid());
}

void PsService::ResetInactivePdpContext(Message *pMsg) {
    PdpContext* pdpContext = NULL;
    SetupDataCallRequestData *rildata = (SetupDataCallRequestData *)GetRequestData(pMsg);

    if (rildata != NULL) {
        pdpContext= rildata ->getActivatingPdpCtx();
        RilLogE("unused requested cid : %d", pdpContext->GetCID());
        pdpContext->Reset();
    }
}

bool PsService::isEqualCid(Message *pMsg, int rspCid) {
    PdpContext* pdpContext = NULL;
    int requestedCid = -1;
    SetupDataCallRequestData *rildata = (SetupDataCallRequestData *)GetRequestData(pMsg);

    if(rildata != NULL) {
        pdpContext= rildata ->getActivatingPdpCtx();
        if(pdpContext !=NULL) {
            requestedCid = pdpContext->GetCID();
            RilLogE("requested cid : %d, current responed cid %d", requestedCid, rspCid);
            return (rspCid == requestedCid);
        } else {
            RilLogE("requestedPdpCtx is NULL");
            return true;
        }
    }

    return true;
}

template <typename T>
int PsService::OnSetupDataCallDone(Message *pMsg) {
    RilLogI("[%s] %s() T=%s", m_szSvcName, __FUNCTION__, __PRETTY_FUNCTION__);

    if (IsNullResponse(pMsg)) return -1;

    ModemData *pModemData = pMsg->GetModemData();
    pModemData = m_pTestHandler->replaceModemDataForTest(pModemData, 0);

    T adapter(pModemData);

    OnSetupDataCallDoneInternal(adapter, pMsg);

    return 0;
}

template<typename T>
void PsService::OnSetupDataCallDoneInternal(T &adapter, Message *pMsg) {
    int errorCode = adapter.GetErrorCode();

    errorCode = m_pTestHandler->forceFailOnceForTest(errorCode);

    if (errorCode == RIL_E_SUCCESS) {
        int cid = adapter.GetCid();
        const DataCall *pDc = adapter.GetDataCall();
        PdpContext *pPdpContext = GetPdpContext(cid);

        int halVer = getHalVersion(pMsg);

        PsUtil::PrintDataCallInfo(pDc, GetServiceName(), __FUNCTION__, halVer);

        int active = adapter.GetActiveStatus();

        if (active != INACTIVE) {
            // print IP address information
            PsUtil::PrintAddressInfo(pDc);

            // OnActivated PDP Context
            if (pPdpContext != NULL && pPdpContext->OnActivated(pDc) == 0) {
                // Update P-CSCF address, Use Embedded values in RIL Response
                // TBD: Update MTU, PCO
                // TBD: Check IP Changing
               if (cid < MAX_CID_RANGE && !isEqualCid(pMsg, cid)) {
                   RilLogE("PDP Context : PDP_ERROR_CID_DIFF token: 0x%x", pMsg->GetToken());
                   ResetInactivePdpContext(pMsg);
               }
            } else {
                RilLogE("PDP Context : OnActivated error");
            }
        } else {
            // pPdpContext can be null when cid is out of bound.
            if (pPdpContext) pPdpContext->UpdateDataCallInfo(pDc);
            RilLogE("PDP is inactive state");
        }

        OnSetupDataCallComplete(RIL_E_SUCCESS, pPdpContext, pMsg);
    } else {
        RilLogW("========== Warning, W/A OnSetupDataCallDone error ==========)");
        RilLogW("ErrorCode=0x%0X", errorCode);
        // For SetupDataCall async model, CP should check this part. This shouldn't be activated but for debugging.
        // For backward compatibility, reset PDP context.
        PdpContext* pdpContext = NULL;
        SetupDataCallRequestData *rildata = (SetupDataCallRequestData *)GetRequestData(pMsg);
        if (rildata != NULL) {
            pdpContext  = rildata ->getActivatingPdpCtx();
        }
        OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, pdpContext, pMsg);
    }
    // Set Fast Dormancy timer info after PDP activation done
    m_pSetupDataCallHandler->DoSetFastDormancy();
}

void PsService::OnSetupDataCallComplete(int errorCode, PdpContext *pPdpContext, Message *pMsg) {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    using namespace std::placeholders;

    int halVer = getHalVersion(pMsg);

    if (errorCode == RIL_E_SUCCESS) {
        if (pPdpContext != NULL) {
            int state = pPdpContext->GetState();

            int active = pPdpContext->GetActive();
            RilLogV("[%s] state=%d, active=%d", __FUNCTION__, state, active);

            if (state == PDP_CONTEXT_CONNECTED && active >= ACTIVE_AND_LINKDOWN) {
                PsDataBuilder builder(halVer);
                builder.setIpv4DnsQueryFirst(needDnsV4First());
                const RilData *rildata = builder.BuildSetupDataCallResponse(errorCode, pPdpContext);
                DataCall *dc = pPdpContext->GetDataCallInfo();
                if (rildata != NULL) {
                    RilLogW("[%s] %s() Verifying RilData", GetServiceName(), __FUNCTION__);
                    RilLogV("  Length=%d, sizeof(v11)=(%d), sizof(1_6)=(%d)", rildata->GetDataLength(), sizeof(RIL_Data_Call_Response_v11), sizeof(RIL_SetupDataCallResult_V1_6));
                    if(halVer >= HAL_VERSION_CODE(1, 6)) {
                        PsUtil::PrintRilDataInfo(&((RIL_SetupDataCallResult_V1_6 *)rildata->GetData())[0], __FUNCTION__);
                    } else {
                        PsUtil::PrintRilDataInfo(&((RIL_Data_Call_Response_v11 *)rildata->GetData())[0], __FUNCTION__);
                    }

                    notifyAMBR(pPdpContext->GetApnSetting(), dc);

                    OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength(), pMsg);
                    delete rildata;

                    // notify data call list
                    OnNotifyDataCallList();
                    return;
                }
            }
        }
    }

    RilLogV("[%s] %s errorCode is %d, pPdpContext=%p", GetServiceName(), __FUNCTION__, errorCode, pPdpContext);
    // send a result as SUCCESS even if status is not PDP_FAIL_NONE.
    // status, suggestedRetryTime is determined in BuildSetupDataCallResponse.
    PsDataBuilder builder(halVer);
    const RilData *rildata = builder.BuildSetupDataCallResponse(errorCode, pPdpContext);
    if (rildata != NULL) {
        OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength(), pMsg);
        delete rildata;
    }

    // In case ERROR or fail to request complete even if errorCode is SUCCESS.
    if (pPdpContext != NULL) tryDeactPdpOnAbnormalState(pPdpContext, errorCode);
}

// This will not be used before SendRequest, don't care about async call
void PsService::OnSetupDataCallCompletePdpFail(int pdp_fail_status, Message *pMsg) {
    RilLogV("[%s] %s PDP_FAIL status is %d", GetServiceName(), __FUNCTION__, pdp_fail_status);
    // send a result as SUCCESS even if status is not PDP_FAIL_NONE.
    // status, suggestedRetryTime is determined in BuildSetupDataCallResponse.
    int halVer = getHalVersion(pMsg);

    PsDataBuilder builder(halVer);
    const RilData *rildata = builder.BuildSetupDataCallResponse(RIL_E_GENERIC_FAILURE, pdp_fail_status);
    if (rildata != NULL) {
        OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength(), pMsg);
        delete rildata;
    }
}

void PsService::OnNotifyDataCallList() {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    RilLogV("********** Prepare DataCall *************");
    PsDataCallListBuilder builder(RilApplication::RIL_HalVersionCode);
    builder.setIpv4DnsQueryFirst(needDnsV4First());
    builder.SetCurRrcState(IsRrcConState());

    for (int i = 0; i < m_nPdpContextSize; i++) {
        PdpContext *pPdpContext = GetPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            RilLogV("[%d] PDP Context{cid=%d,state=%d,active=%d}",
                    i, pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());

            if (pPdpContext->GetState() == PDP_CONTEXT_CONNECTED) {
                pPdpContext->SetQosSession();
                builder.AddDataCall(pPdpContext);
            }

            DataCall *dc = pPdpContext->GetDataCallInfo();
            notifyAMBR(pPdpContext->GetApnSetting(), dc);
        }
    } // end for i ~
    RilLogV("****************************************");

    int halVer = RilApplication::RIL_HalVersionCode;
    const RilData *rildata = builder.Build(halVer);
    if (rildata != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_DATA_CALL_LIST_CHANGED, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }
}

int PsService::OnSetupDataCallTimeout(Message *pMsg) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    SetupDataCallRequestData *rildata = NULL;
    if (!GetRilRequestData(pMsg, rildata)) return -1;

    PdpContext* pActPdpCtx = rildata->getActivatingPdpCtx();

    // complete connecting PDP context
    if (pActPdpCtx != NULL) {
        RilLogW("Reset Activating PDP Context CID=%d", pActPdpCtx->GetCID());
    } else
        RilLogW("Timeout DoSetupDataCall but no Activating PDP Context");

    OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, pActPdpCtx, pMsg);

    return 0;
}

void PsService::OnDeactDataCallComplete(PdpContext *pPdpContext) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pPdpContext != NULL) {
        // clear P-CSCF address information
        // disconnect
        RilLogV("Last Deact PDP Context : CID=%d", m_pDeactivatingPdpContext->GetCID());
        pPdpContext->OnDeactivated();
    } else {
        RilLogW("PDP Context is null");
        RilLogW("PDP Context may be released by OnDataCallListChanged");
    }
    OnRequestComplete(RIL_E_SUCCESS);
    OnNotifyDataCallList();
    m_pDeactivatingPdpContext = NULL;
}

int PsService::OnRequestDeactDataCall(PdpContext *pPdpContext, int reason/* = DEACT_REASON_NORMAL*/) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pPdpContext == NULL) {
        RilLogE("pPdpContext is NULL");
        return -1;
    }

    int state = pPdpContext->GetState();
    RilLogV("PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
    switch (state) {
    case PDP_CONTEXT_CONNECTING:
    case PDP_CONTEXT_CONNECTED:
        break;
    default:
        RilLogW("Unexpected PDP Context state");
        return -1;
    }

    int cid = pPdpContext->GetCID();
    return OnRequestDeactDataCall(cid, reason);
}

int PsService::OnRequestDeactDataCall(int cid, int reason/* = DEACT_REASON_NORMAL*/) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildDeactDataCall(cid, reason);
    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) RilLogW("Fail to send PDP Deact request ");

        delete pModemData;
    }
    return 0;
}

int PsService::OnSimpleRequestDone(Message *pMsg) {
    if (IsNullResponse(pMsg)) return -1;

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS && pMsg->GetMsgId() == MSG_PS_SET_DATA_PROFILE_DONE) m_bIsAttachDone = true;

    OnRequestComplete(errorCode);

    return 0;
}

/* Only used for Internet Notification, not for VZW */
int PsService::DoSetDataProfileInternal(SetupDataCallRequestData *rildata) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    RilLogV("[%d] DataProfile APN = %s", GetRilSocketId(), rildata->GetApn());

    DataV2_0::RIL_DataProfileInfo dpi = rildata->mDataProfileInfo;
    dpi.profileId = (RIL_DataProfileId)INTERNET_FOR_CP; // CP Expects 0xFF which is used for Internet APN

    DataV2_0::RIL_DataProfileInfo *dpi_ptr[1];
    dpi_ptr[0] = &rildata->mDataProfileInfo;

    RequestData *pData = RilParser::CreateSetDataProfile(RIL_REQUEST_SET_DATA_PROFILE, 0,
                                                                (char *)&dpi_ptr, sizeof(RIL_DataProfileInfo_V1_5 *));
    if (pData != NULL) {
        Message *msg = Message::ObtainMessage(pData, RIL_SERVICE_PS, MSG_PS_SET_DATA_PROFILE);
        if (GetRilContext()->GetServiceManager()->SendMessage(msg) < 0) {
            if (msg) {
                delete msg;
            }
        }
    }
    return 0;
};

bool PsService::IsPossibleToPassInRadioOffState(int request_id) {
    switch (request_id) {
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
        case RIL_REQUEST_SET_DATA_PROFILE:
        case RIL_REQUEST_SET_PREFERRED_DATA_MODEM:
        case RIL_REQUEST_OEM_DETACH:
        case RIL_REQUEST_OEM_SET_URSP:
        case RIL_REQUEST_ALLOCATE_PDU_SESSION_ID:
        case RIL_REQUEST_RELEASE_PDU_SESSION_ID:
            RilLogE("IsPossibleToPassInRadioOffState cmd %d", request_id);
            break;
        default:
            return false;
    }
    return true;
}

int PsService::isSameTargetOperator(int targetOperator) {
    int nTargetOperator = 999;
    RilProperty* appProperty = GetRilApplicationProperty();
    if ( appProperty != NULL ) nTargetOperator = appProperty->GetInt(RIL_APP_TARGET_OPER);
    if ( targetOperator == nTargetOperator ) return true;
    return false;
}

inline bool PsService::IsNullMsg(Message *pMsg) {
    if(pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return  true;
    }
    return false;
}

bool PsService::IsNullResponse(Message *pMsg) {
    if(NULL == pMsg || NULL == pMsg->GetModemData())
    {
        RilLogE("[%s] msgId:%d, pMsg is NULL or modemdata data is NULL",
                GetServiceName(), (pMsg) ? pMsg->GetMsgId() : 0);
        return true;
    }
    return false;
}

void PsService::notifyAMBR(ApnSetting *pApnSetting, const DataCall *out) {
    // Notify AMBR on Internet success case
    if( NULL == pApnSetting || !pApnSetting->CanHandleType(APN_TYPE_BIT_DEFAULT)) return;

    // Check AMBR
    PsUtil::decodeAMBR(out);

    // Send AMBR through OEM RIL INDICATION
    // struct AMBR == sit_pdp_data_call_item_ext_ambr
    RilLogV("notify AMBR to framework : CID:%d, len:%d", out->cid, out->ambr.octet2);
    if(out->ambr.octet2 != 0) OnUnsolicitedResponse(RIL_UNSOL_OEM_AMBR, (void *)&out->ambr, sizeof(AMBR));
}

template<typename T>
bool PsService::GetRilRequestData(Message *pMsg, T *(&rildata)) {
    if (IsNullMsg(pMsg)) return false;

    rildata = (T *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return false;
    }
    return true;
}

bool PsService::cleanUpPdpOnInactiveDc(const DataCall *dc)
{
    if (dc->active == INACTIVE) {
        PdpContext *pPdpContext = GetPdpContext(dc->cid);
        if (pPdpContext != NULL) {
            RilLogV("PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
            if (pPdpContext->GetState() == PDP_CONTEXT_CONNECTED || pPdpContext->GetActive() >= ACTIVE_AND_LINKDOWN)
                pPdpContext->OnDeactivated();
            else if (pPdpContext->GetState() == PDP_CONTEXT_CONNECTING)
                OnSetupDataCallComplete(RIL_E_GENERIC_FAILURE, pPdpContext, NULL);
        }
        return true;
    }
    return false;
}

bool PsService::updatePdpOnActiveDc(const DataCall *dc)
{
    if (dc->active != INACTIVE) {
        RilLogI("DataCall(cid=%d) from CP is Active", dc->cid);
        PdpContext *pPdpContext = GetPdpContext(dc->cid);
        // For invalid cid, we just skip it
        if(pPdpContext == NULL) return true;
        pPdpContext->SetActive(dc->active);
        pPdpContext->OnChanged(dc); // This will update Matched PdpContext then reconfigure IP address
        return true;
    }
    return false;
}

int PsService::makePermanentFailureForIMS(PdpContext *pPdpContext) {
    // Network/CP reported DataCall is lost, AOSP can treat as retrying flow.
    // But for IMS/EIMS, Retrying by DcTracker/RetryManager is not expected.
    // So keep DC and with changing active state to inactive and permanent failure cause
    if(pPdpContext->GetActive() >= ACTIVE_AND_LINKDOWN) {
        ApnSetting *apnSetting = pPdpContext->GetApnSetting();
        if (apnSetting && (apnSetting->CanHandleType(APN_TYPE_BIT_IMS) ||
                           apnSetting->CanHandleType(APN_TYPE_BIT_EMERGENCY)) ) {
            pPdpContext->SetActive(INACTIVE);
            DataCall *old_dc = pPdpContext->GetDataCallInfo();
            old_dc->status = PDP_FAIL_MISSING_UNKNOWN_APN;
            pPdpContext->UpdateDataCallInfo(old_dc);
        }
        //Keep DataCall num for IMS / EIMS
        return 1;
    }
    return 0;
}

void PsService::cleanUpInactivePdp(PdpContext *pPdpContext) {
    // RIL active / CP inactive -> update state
    if (pPdpContext->GetActive() >= ACTIVE) {
        RilLogV("DataCall Mismatch is detected. Cleanup PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
        pPdpContext->OnDeactivated();
    }
}

void PsService::tryDeactPdpOnAbnormalState(PdpContext *pPdpContext, int errorCode) {
    RilLogV("PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
    int state = pPdpContext->GetState();
    int active = pPdpContext->GetActive();
    bool deact = (state == PDP_CONTEXT_CONNECTING && (active >= ACTIVE_AND_LINKDOWN || errorCode != RIL_E_SUCCESS)) ||
        (state == PDP_CONTEXT_CONNECTED);
    // VZ_REQ_LTEDATARETRY_7783
    int reason = DEACT_REASON_NORMAL;
    if (deact) {
        RilLogW("Deactivate selected PDP Context");

        DataCall *dc = pPdpContext->GetDataCallInfo();
        static unsigned char nullIpv4[MAX_IPV4_ADDR_LEN] = { 0, };
        //recovery for IPv4 0000, set deact reason as PDP reset
        if ((dc->pdpType != PDP_TYPE_IPV6) && (memcmp(nullIpv4, dc->ipv4.addr, sizeof(nullIpv4)) == 0)) {
            RilLogW("IPv4 is 0000, DEACT_REASON_PDP_RESET");
            reason = DEACT_REASON_PDP_RESET;
        }
        OnRequestDeactDataCall(pPdpContext, reason);
    }
    pPdpContext->OnDeactivated();
}

// Default is DNSV6 first if both IPv4v6 is provided
bool PsService::needDnsV4First() {
    if(SystemProperty::GetInt(RIL_VENDOR_RADIO_DNSV4_FIRST, 0) == 1)
        return true;
    if(isSameTargetOperator(TARGET_OPER_CMCC) || isSameTargetOperator(TARGET_OPER_CHNOPEN))
        return true;

    return false;
}

int PsService::getHalVersion(Message *pMsg) {
    int halVer = RilApplication::RIL_HalVersionCode;
    RequestData *requestedData = (RequestData *)GetRequestData(pMsg);
    if (requestedData != NULL) {
        halVer = requestedData->GetHalVersion();
        RilLogV("[%s] halVersion = 0x%x", __FUNCTION__, halVer);
    }
    return halVer;
}

void PsService::dump(std::stringstream& ss) {
    ss << GetServiceName() << ":{"<< "\n";
    for (int i = 0; i < m_nPdpContextSize; i++) {
        PdpContext *pPdpContext = GetPdpContextByIndex(i);
        if(pPdpContext != NULL){
            ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
            if ((pPdpContext->GetState() != PDP_CONTEXT_UNAVAILABLE) && (pPdpContext->GetState() != PDP_CONTEXT_DISCONNECTED)) {
                ss << pPdpContext->ToString();
                if(pApnSetting != NULL) {
                    ss << ", ";
                    ss << pApnSetting->ToString();
                }
                ss << "\n";
            }
        }
    } // end for i ~
    ss << "}";
}

void PsService::printDataProfileTable() {
    RilLogV("[%d] printDataProfileTable database. table size : %zu", GetRilSocketId(), m_DataProfileInfo.size());

    for(unsigned int i = 0; i < m_DataProfileInfo.size(); i++) {
        RilLogV("[printDataProfileTable] %s", m_DataProfileInfo[i].toString().c_str());
    }
}

void PsService::clearDataProfileTable() {
    RilLogV("[%d] clearAllDataProfile database", GetRilSocketId());
    m_DataProfileInfo.clear();
}

void PsService::addDataProfile(const DataV2_0::RIL_DataProfileInfo &dpi) {
    RilLogV("[%d] addDataProfile : %s", GetRilSocketId(), dpi.apn.c_str());
    m_DataProfileInfo.push_back(dpi);
}

DataV2_0::RIL_DataProfileInfo * PsService::findDataProfile(const char * apn) {
    //for(auto it = m_DataProfileInfo.begin(); it != m_DataProfileInfo.end(); it++) {
    for(unsigned int i = 0; i < m_DataProfileInfo.size(); i++) {
        if(TextUtils::Equals(apn, m_DataProfileInfo[i].apn)) {
            RilLogV("[%d] findDataProfile[%d] : Must be found : %s, Found : %s", GetRilSocketId(), i, m_DataProfileInfo[i].apn.c_str(), apn);
            return &m_DataProfileInfo[i];
        }
    }

    RilLogV("[%d] findDataProfile : The APN(%s) does not exist in the list.", GetRilSocketId(), apn);
    return nullptr;
}
