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
 * SimIoService.cpp
 *
 */

#include "simioservice.h"
#include <librilutils/textutils.h>
#include "rillog.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "simdata.h"
#include "simdatabuilder.h"
#include "util.h"
#include <librilutils/systemproperty.h>
#include "servicemgr.h"
#include "mcctable.h"
#include "rilparser.h"
#include "MccMncChanger.h"
#include "simservice.h"
#include "simlockhandler.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIMIO, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIMIO, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIMIO, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIMIO, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)     { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)       { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)       { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()        { RilLogI("%s::%s() [<-- ", m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()        { RilLogI("%s::%s() [--> ", m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()     { RilLogE("%s::%s() Not Implemented", m_szSvcName, __FUNCTION__); }

#define LOGI(format, ...)   RilLogI("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)   RilLogV("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)   RilLog("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)   RilLogE("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)

enum _ImsiType {
    GSM,
    CDMA
};

SimIoService::SimIoService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_SIM_IO)
{
    m_pSimIoData = NULL;
}

SimIoService::~SimIoService()
{
    if (m_pSimIoData != NULL) {
        delete m_pSimIoData;
        m_pSimIoData = NULL;
    }
}

int SimIoService::OnCreate(RilContext * /* pRilContext */)
{
    ENTER_FUNC();
    LEAVE_FUNC();
    return 0;
}

BOOL SimIoService::OnHandleRequest(Message* pMsg)
{
    if (pMsg == NULL) {
        return FALSE;
    }

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_IO:
        ret = DoSimIo(pMsg);
        break;
    case MSG_SIM_GET_IMSI:
        ret = DoGetImsi(pMsg);
        break;
    case MSG_SIM_OEM_IMS_SIM_IO:
        ret = DoOemImsSimIo(pMsg);
        break;
    default:
        // TODO log unsupported message id
        return FALSE;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL SimIoService::OnHandleSolicitedResponse(Message* pMsg)
{
    PARAM_NULL(pMsg);

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_VERIFY_PIN2_DONE:
        ret = OnVerifyPin2Done(pMsg);
        break;
    case MSG_SIM_IO_DONE:
        ret = OnSimIoDone(pMsg);
        break;
    case MSG_SIM_GET_IMSI_DONE:
        ret = OnGetImsiDone(pMsg);
        break;
    case MSG_SIM_OEM_IMS_SIM_IO_DONE:
        ret = OnOemImsSimIoDone(pMsg);
        break;
    default:
        // TODO log unsupported message id
        return FALSE;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

void SimIoService::OnSimStatusChanged(int cardState, int /* appState */)
{
    if (cardState != RIL_CARDSTATE_PRESENT) {
        // IMSI is not valid anymore
        UpdateImsi("");

        // notify an empty SIM operator numeric to OEM clients
        OnUnsolicitedResponse(RIL_UNSOL_OEM_SIM_OPERATOR_NUMERIC);
    }
}

int SimIoService::OnVerifyPin2Done(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());

    RilContext *pContext = GetRilContext();
    if (pContext) {
        SimService *pSimService = (SimService *) pContext->GetService(RIL_SERVICE_SIM);
        SimLockHandler *pSimLockHandler = pSimService->GetSimLockHandler();
        pSimLockHandler->SetPinPukRemain(SIM_PIN, PIN2_PUK2, adapter.GetRemainCount());

        UINT uErrCode = adapter.GetErrorCode();
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
        if (pRilData == NULL) {
            return OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }

        if (uErrCode == RIL_E_SUCCESS) {
            // PIN2 Verification for FDN update
            if (m_pSimIoData != NULL) {
                if (SendRequest(m_pSimIoData, TIMEOUT_SIMIO_DEFAULT, MSG_SIM_IO_DONE) == -1) {
                    OnRequestComplete(RIL_E_GENERIC_FAILURE);
                }

                //delete m_pSimIoData;
                m_pSimIoData = NULL;
            } else {
                RilLogE("%s(): Invalid scenario : uErrCode :%d", __FUNCTION__, uErrCode);
            }
        } else {
            if (m_pSimIoData != NULL) {
                OnRequestComplete(RIL_E_SIM_PIN2);

                delete m_pSimIoData;
                m_pSimIoData = NULL;
            } else {
                RilLogE("%s(): Invalid scenario : uErrCode :%d", __FUNCTION__, uErrCode);
            }
        }

        if (NULL != pRilData) delete pRilData;
    }
    LEAVE_FUNC();
    return 0;
}

int SimIoService::DoSimIo(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = -1;

    SimIoData *rildata = (SimIoData *)pMsg->GetRequestData();

    int nAppType = RIL_APPTYPE_UNKNOWN;

    RilContext *pContext = GetRilContext();
    if (pContext) {
        SimService *pSimService = (SimService *) pContext->GetService(RIL_SERVICE_SIM);
        V2_0::RIL_CardStatus status = pSimService->GetRilCardStatus();
        RilLogV("%s::%s() RIL AID:%s, App Num:%d", m_szSvcName, __FUNCTION__
                , rildata->m_strAid, status.num_applications);
        if (0 < strlen(rildata->m_strAid)) {
            for (int i = 0; i < status.num_applications; i++) {
                RilLogV("%s::%s() %d. Stored AID:%s", m_szSvcName, __FUNCTION__
                        , i, status.applications[i].aid_ptr);

                if (status.applications[i].aid_ptr != NULL
                        && strcmp(rildata->m_strAid, status.applications[i].aid_ptr) == 0) {
                    nAppType = status.applications[i].app_type;
                    break;
                }
            }
        }
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimIO(rildata->m_nCmd, nAppType,
            rildata->m_nFileId, rildata->m_strPath, rildata->m_nP1, rildata->m_nP2, rildata->m_nP3,
            rildata->m_nDataLen, rildata->m_strData, rildata->m_strPin2, rildata->m_strAid);

    // UPDATE_BINARY, UPDATE_RECORD
    UINT uResultMsg = 0;
    // PIN2 for FDN
    if (SIM_EFID_FDN == rildata->m_nFileId && (rildata->m_nCmd == 0xD6 || rildata->m_nCmd == 0xDC)
            && 0 < strlen(rildata->m_strPin2)) {
        if(m_pSimIoData!=NULL) delete m_pSimIoData;
        m_pSimIoData = pModemData;

        pModemData = builder.BuildSimVerifyPin(SimService::PIN2_PUK2, rildata->m_strPin2, rildata->m_strAid);
        uResultMsg = MSG_SIM_VERIFY_PIN2_DONE;
    }
    else uResultMsg = MSG_SIM_IO_DONE;

    if (pModemData != NULL) nResult = SendRequest(pModemData, TIMEOUT_SIMIO_DEFAULT, uResultMsg);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int SimIoService::OnSimIoDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    ProtocolSimResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        ProtocolSimIOAdapter adapter(pMsg->GetModemData());

        SimIoData *rildata = (SimIoData *)GetCurrentMsg()->GetRequestData();
        if (rildata != NULL) {
            // Fetch a correct MNC length from IMSI using EF_AD payload
            if (rildata->m_nCmd == COMMAND_READ_BINARY && rildata->m_nFileId == EF_AD) {
                IccIoResult iccioResult(adapter.GetSw1(), adapter.GetSw2()
                        , (char *)adapter.GetResponse(), adapter.GetResponseLength());
                // USIM only
                OnHandleEF_AD(iccioResult);
            }
        } else {
            RilLogV("[%s] transaction had been completed or not acting", GetServiceName());
        }

        //SimIoData *pSimIoData = (SimIoData *)pMsg->GetRequestData();
        UINT uRilResult = RIL_E_SUCCESS;
        if (adapter.GetSw1() != 0x90 && adapter.GetSw1() != 0x91 && adapter.GetSw1() != 0x9e
                && adapter.GetSw1()!=0x9f) {
            uRilResult = RIL_E_GENERIC_FAILURE;
        }

        SimDataBuilder builder;
        const RilData *pRilData = NULL;
        if (rildata != NULL && rildata->m_nCmd == COMMAND_STATUS) {
            pRilData = builder.BuildSimIoFcpTemplateResponse(adapter.GetSw1(), adapter.GetSw2()
                    , adapter.GetResponseLength(), adapter.GetResponse());
        } else {
            pRilData = builder.BuildSimIoResponse(adapter.GetSw1(), adapter.GetSw2()
                    , adapter.GetResponseLength(), adapter.GetResponse());
        }

        if (pRilData == NULL) {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        } else {
            OnRequestComplete(uRilResult, pRilData->GetData(), pRilData->GetDataLength());
            if (pRilData != NULL) delete pRilData;
        }

    }
    else OnRequestComplete(uErrCode);

    LEAVE_FUNC();
    return 0;
}

int SimIoService::DoGetImsi(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = -1;

    StringsRequestData *rildata = (StringsRequestData *)pMsg->GetRequestData();
    const char *aid = rildata->GetString(0);
    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildGetImsi(aid);
    nResult = SendRequest(pModemData, TIMEOUT_SIMIO_DEFAULT, MSG_SIM_GET_IMSI_DONE);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int SimIoService::OnGetImsiDone(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimImsiAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
#ifdef SUPPORT_CHANGE_MCCMNC
        const char *imsi = checkMmcMnc(adapter.GetImsi());
#else
        const char *imsi = adapter.GetImsi();
#endif
        // update IMSI
        if (GetCurrentRequestData() != NULL) {
            const char *aid = ((StringsRequestData *)GetCurrentRequestData())->GetString(0);
            UpdateImsi(aid, imsi, GSM);
        }
        else {
            RilLogW("[%s]%s Invalid IMSI transaction.", GetServiceName(), __FUNCTION__);
        }

        SimDataBuilder builder;
        const RilData *rildata = builder.BuildGetImsiResponse(imsi);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
#ifdef SUPPORT_CHANGE_MCCMNC
        if (imsi != NULL) {
            delete[] imsi;
        }
#endif
    }
    else OnRequestComplete(uErrCode);

    LEAVE_FUNC();
    return 0;
}

#ifdef SUPPORT_CHANGE_MCCMNC
char* SimIoService::checkMmcMnc(const char* pImsi) {
    MccMncChanger changer;
    char *pMccMnc = new char[6];
    char *pCopiedImsi = new char[MAX_IMSI_LEN + 1];
    strncpy(pCopiedImsi, pImsi, MAX_IMSI_LEN + 1);
    string pszImsi = pImsi;
    bool success = changer.checkMccMncToChange((pszImsi.substr(0, 6)).c_str(), pMccMnc);
    if (true == success && 0 != strcmp((pszImsi.substr(0, 6)).c_str(), pMccMnc)) {
        strncpy(pCopiedImsi, pMccMnc, 6);
        RilLogV("[%s] %s(): checked IMSI: [%s]", m_szSvcName, __FUNCTION__, pCopiedImsi);
    } else {
        RilLogV("[%s] %s(): checked IMSI doesn't need to be changed", m_szSvcName, __FUNCTION__);
    }

    delete[] pMccMnc;
    return pCopiedImsi;
}
#endif

string SimIoService::FetchSimOperator(IccIoResult &iccioResult)
{
    string carrier = "";

    // fetch mcc length from EF_AD
    // use low 4bit of 4th byte in EF_AD data
    // 1.mncLength is 2 or invalid, try to look up
    //   1-1. MCCMNC_CODES_HAVING_3DIGITS_MNC
    //   1-2. MCC based smallest mnc length
    // 2.mncLength is more than 2 (normally 3), copy mcc 3 digit and mnc 3 digit.
    if (iccioResult.IsSuccess()) {
        int mncLength = -1;
        if (iccioResult.GetDataLength() > 3 && iccioResult.GetData() != NULL) {
            mncLength = *(iccioResult.GetData() + 3) & 0xF;
        }

        if ((mncLength < 0 || mncLength == 0xF || mncLength == 2) && mImsi.length() >= 6) {
            if (MccTable::GetSmallestDigitsMccForImsi(mImsi) > 0) {
                mncLength = MccTable::GetSmallestDigitsMccForImsi(mImsi);
            }

            if (mncLength < 0 || mncLength == 0xF) {
                mncLength = MccTable::GetSmallestDigitsMccForMcc(mImsi.substr(0, 3));
            }
        }

        string mccmnccode = mImsi.substr(0, 6);
        if (2 <= mncLength) {
            carrier = mccmnccode.substr(0, 3 + mncLength);
            RilLogV("[%s] mncLength=%d carrier=%s", GetServiceName(), mncLength, carrier.c_str());
        } else {
            RilLogW("[%s] EF_AD corrupted or invalid mnc length=%d. IMSI=%sXXXXXXXXX"
                    , GetServiceName(), mncLength, mccmnccode.c_str());
        }
    }

    return carrier;
}

void SimIoService::UpdateImsi(const char *imsi)
{
    UpdateImsi(NULL, imsi, GSM);
}

void SimIoService::UpdateImsi(const char *aid, const char *imsi, int imsiType)
{
    RilContext *pContext = GetRilContext();
    if (pContext) {
        // store IMSI in RilContext Property
        RilProperty *property = GetRilContextProperty();
        char imsiforVapp[40] = {0,};
        RilLogV("[%d]%s imsiType : %s", GetRilSocketId(), __FUNCTION__, (imsiType == GSM) ? "GSM" : "CDMA");
        if (property != NULL) {
            if (imsiType == GSM) {
                // legacy
                property->Put(RIL_CONTEXT_SIM_IMSI, imsi);
                // IMSI and AID for USIM/SIM
                property->Put(RIL_CONTEXT_GSM_SIM_AID, aid);
                property->Put(RIL_CONTEXT_GSM_SIM_IMSI, imsi);
                sprintf(imsiforVapp, "vendor.ril.%s_%d", RIL_CONTEXT_GSM_SIM_IMSI, GetRilSocketId());
                SystemProperty::Set(imsiforVapp, imsi);
            } else if (imsiType == CDMA) {
                // IMSI and AID for CSIM/RUIM
                property->Put(RIL_CONTEXT_CDMA_SIM_AID, aid);
                property->Put(RIL_CONTEXT_CDMA_SIM_IMSI, imsi);
                sprintf(imsiforVapp, "vendor.ril.%s_%d", RIL_CONTEXT_CDMA_SIM_IMSI, GetRilSocketId());
                SystemProperty::Set(imsiforVapp, imsi);
            } else {
                RilLogE("[%d]%s Invalid IMSI type (%d)", GetRilSocketId(), __FUNCTION__, imsiType);
            }
        }

        if (aid == NULL || imsiType == GSM) {
            string oldImsi = mImsi;
            mImsi = (imsi != NULL) ? imsi : "";
            string printableImsi = (mImsi.length() >= 6) ? mImsi.substr(0, 6) + "XXXXXXXX" : "";
            RilLogI("[%d]IMSI updated: AID=%s IMSI=%s", GetRilSocketId(), aid, printableImsi.c_str());
        } else {
            RilLogV("[%d]%s AID(%s) is not for (U)SIM subscription.", GetRilSocketId(), __FUNCTION__, aid);
        }
    } else {
        RilLogE("[%d]%s pContext is NULL", GetRilSocketId(), __FUNCTION__, aid);
    }
}

bool SimIoService::IsPossibleToPassInRadioOffState(int request_id)
{
    switch (request_id) {
        case RIL_REQUEST_SIM_IO:
        case RIL_REQUEST_GET_IMSI:
        case RIL_REQUEST_OEM_IMS_SIM_IO:
            break;
        default:
            return false;
    }
    return true;

}

void SimIoService::SendOpenCarrierInfoRilReq(const char *plmn)
{
    char info[2][15];
    char **ptrStrings;
    unsigned int len, openCarrierIndex;

    ptrStrings = (char **)new char*[2];
    memset(ptrStrings, 0, 2*sizeof(char *));

    memset(info, 0, sizeof(info));
    openCarrierIndex = GetOpenCarrierIndex();
    len = snprintf(info[0], 15, "%d", openCarrierIndex);
    memcpy(info[1], plmn, strlen(plmn));

    ptrStrings[0] = info[0];
    ptrStrings[1] = info[1];

    RequestData *pData = RilParser::CreateStrings(RIL_REQUEST_SET_OPEN_CARRIER_INFO, 0
            , (char*)ptrStrings, 2 * sizeof(char *));

    if (pData != NULL) {
        Message *pMsg = Message::ObtainMessage(pData, RIL_SERVICE_MISC, MSG_MISC_SET_OPEN_CARRIER_INFO);
        ServiceMgr *pServigMrg = GetRilContext()->GetServiceManager();
        if (pServigMrg != NULL) {
            if (pServigMrg->SendMessage(pMsg) < 0) {
                RilLogV("%s(): SendMessage error", __FUNCTION__);
                delete pMsg;
            }
        } else if(pMsg != NULL) {
            delete pMsg;
        }
    }

    delete[] ptrStrings;
}

// IMS
int SimIoService::DoOemImsSimIo(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildOemSimRequest(RIL_REQUEST_SIM_IO
            , (BYTE *)rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIMIO_DEFAULT, MSG_SIM_OEM_IMS_SIM_IO_DONE);

    LEAVE_FUNC();
    return nResult;
}

int SimIoService::OnOemImsSimIoDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    ProtocolSimResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode, (void *) adapter.GetParameter(), adapter.GetParameterLength());

    LEAVE_FUNC();
    return 0;
}

void SimIoService::OnHandleEF_AD(IccIoResult &iccioResult)
{
    string carrier = FetchSimOperator(iccioResult);
    if (carrier.length() == 5 || carrier.length() == 6) {
        char numeric[32] = { 0, };
        strncpy(numeric, carrier.c_str(), carrier.length());
        RilProperty *pProperty = GetRilContextProperty();
        if (pProperty != NULL) {
            pProperty->Put(RIL_CONTEXT_SIM_OPERATOR, numeric);
        }

        // set open carrier index
        SetOpenCarrierIndex(numeric);
        SendOpenCarrierInfoRilReq(numeric);

        // notify SIM operator numeric to all OEM clients
        OnUnsolicitedResponse(RIL_UNSOL_OEM_SIM_OPERATOR_NUMERIC, numeric, strlen(numeric));
#ifdef SUPPORT_NOTIFY_DEVICE_INFO
        // GPS service will update and notify
        RilDataStrings *rildata = new RilDataStrings(3);
        if (rildata) {
            rildata->SetString(0, numeric);
            Message *msg = Message::ObtainMessage(rildata, RIL_SERVICE_GPS, MSG_GPS_INTERNAL_UPDATE_DEVICE_INFO);
            if (msg) {
                if (GetRilContext()->GetServiceManager()->SendMessage(msg) < 0) {
                    delete msg;
                }
            }
        }
#endif
    }
}

