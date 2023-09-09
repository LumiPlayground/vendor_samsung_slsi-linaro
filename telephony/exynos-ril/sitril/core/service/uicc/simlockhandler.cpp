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
 * simlockhandler.cpp
 *
 *  Created on: 2020. 9. 3.
 *      Author: hjun78.park
 */

#include "rillog.h"
#include "service.h"
#include "simservice.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "simdata.h"
#include "simdatabuilder.h"
#include "simlockhandler.h"
#include "simautoverifypinhandler.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)         { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)           { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)           { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()            { RilLogI("%s::%s() [<-- ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()            { RilLogI("%s::%s() [--> ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()         { RilLogE("%s::%s() Not Implemented", m_pSimService->m_szSvcName, __FUNCTION__); }

#define LOGI(format, ...)       RilLogI("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)       RilLogV("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)       RilLog("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)       RilLogE("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)

SimLockHandler::SimLockHandler()
{
    // PIN2 for FDN
    m_pEnableFdnData = NULL;
    m_pSimService = NULL;
    m_pSimAutoVerifyPinHandler = NULL;
}

SimLockHandler::~SimLockHandler()
{
    // PIN2 for FDN
    if (m_pEnableFdnData != NULL) {
        delete m_pEnableFdnData;
        m_pEnableFdnData = NULL;
    }
    m_pSimService = NULL;

    InitializePinPuk();
    m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
    m_pSimAutoVerifyPinHandler = NULL;
}

void SimLockHandler::Init(SimService* pSimService)
{
    if (pSimService != NULL) {
        m_pSimService = pSimService;
        m_pSimAutoVerifyPinHandler = pSimService->GetSimAutoVerifyPinHandler();
        // PIN/PUK Verification
        InitializePinPuk();
    }
}

int SimLockHandler::DoSetFacilityLock(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    FacilityLock rildata;

    StringsRequestData *pReq = (StringsRequestData *) pMsg->GetRequestData();
    if (pReq == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", m_pSimService->m_szSvcName, __FUNCTION__);
        LEAVE_FUNC();
        return 0;
    }

    char **ppString = pReq->GetStringsContent();
    if (ppString == NULL || ppString[0] == NULL) { // facility is null
        m_pSimService->OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", m_pSimService->m_szSvcName, __FUNCTION__);
    } else {
        if (rildata.Parse(*pReq) == FALSE) {
            m_pSimService->OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
            RilLogE("%s::%s() Invalid arguments", m_pSimService->m_szSvcName, __FUNCTION__);
            LEAVE_FUNC();
            return 0;
        }

        UINT uResultMsg = 0;
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimSetFacilityLock((char *) rildata.GetCode(),
                rildata.GetLockState(), rildata.GetPassword(), rildata.GetServiceClass(),
                rildata.GetAID());

        RilLogV("%s::%s() FAC(%s), LockState(%d)", m_pSimService->m_szSvcName, __FUNCTION__
                , rildata.GetCode(), rildata.GetLockState());

        if (strcmp(rildata.GetCode(), "SC") == 0) {
            if (rildata.GetLockState() == SIM_FAC_LOCK_STATE_UNLOCK)
                m_pSimAutoVerifyPinHandler->SetAutoPinState(
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLING);
            else if(rildata.GetLockState() == SIM_FAC_LOCK_STATE_LOCK)
                m_pSimAutoVerifyPinHandler->SetAutoPinState(
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING);
        }

        // PIN2 for FDN
        if (strcmp(rildata.GetCode(), "FD") == 0) {
            if (rildata.GetPassword() != NULL && strlen(rildata.GetPassword())>0) {
                if (m_pEnableFdnData != NULL) delete m_pEnableFdnData;

                m_pEnableFdnData = pModemData;
                pModemData = builder.BuildSimVerifyPin(PIN2_PUK2, rildata.GetPassword(),
                        rildata.GetAID());
                uResultMsg = MSG_SIM_VERIFY_PIN2_DONE;
            }
        } else {
            uResultMsg = MSG_SIM_SET_FACILITY_LOCK_DONE;
        }

        if (0 < uResultMsg && pModemData != NULL) {
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, uResultMsg);
        } else {
            if (pModemData != NULL) delete pModemData;
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnSetFacilityLockDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    FacilityLock rildata;
    SimDataBuilder builder;

    ProtocolSimSetFacilityLockAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    Message *pCurrMsg = m_pSimService->GetCurrentMsg();
    if (pCurrMsg != NULL) {
        StringsRequestData *pReq = (StringsRequestData *)pCurrMsg->GetRequestData();
        if (pReq == NULL) {
            RilLogV("%s::%s() Failed to get RequestData.", m_pSimService->m_szSvcName, __FUNCTION__);
        } else if (rildata.Parse(*pReq) == FALSE) {
            RilLogV("%s::%s() Parse() returned False.", m_pSimService->m_szSvcName, __FUNCTION__);
        } else {
            if (strcmp(rildata.GetCode(), "SC") == 0) {
                m_nPinRemain[PIN1_PUK1] = adapter.GetRemainCount();
            } else if (strcmp(rildata.GetCode(), "FD") == 0) {
                m_nPinRemain[PIN2_PUK2] = adapter.GetRemainCount();
            }
        }
    }

    const RilData *pRilData = builder.BuildSimSetFacilityLockResponse(adapter.GetRemainCount());
    if (uErrCode == RIL_E_SUCCESS) {
        if (pRilData != NULL) {
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        } else {
            m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
        if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING) {
#ifdef AUTO_VERIFY_PIN_WITH_CP
#else
        m_pSimAutoVerifyPinHandler->SavePin(rildata.GetPassword());
#endif
        } else if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLING) {
            m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
        }

    } else { // uErrCode != RIL_E_SUCCESS
        if (pRilData != NULL) {
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
        } else {
            m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
        if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLED);
        } else if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLING) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
        }
    }

    if (pRilData != NULL) delete pRilData;

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoVerifyPin(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    VerifyPIN *rildata = (VerifyPIN *)pMsg->GetRequestData();
    const char *pszPIN = rildata->GetPIN();
    const char *pszAID = rildata->GetAID();
    if (pszPIN != NULL && 0 < strlen(pszPIN)) {
        if (IsValidPinPuk(SIM_PIN, pszPIN) == FALSE) {
            SendPasswordInvalid(SIM_PIN, PIN1_PUK1);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimVerifyPin(PIN1_PUK1, pszPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_PIN_DONE);
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING);
        }
    } else {
        //Empty PIN is to get remain count
        LOGV("Empty PIN, Response Remain Count(%d)", m_nPinRemain[PIN1_PUK1]);
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimPinPukResponse(m_nPinRemain[PIN1_PUK1]);
        if (pRilData == NULL) {
            m_pSimService->OnRequestComplete(RIL_E_NO_MEMORY);
        } else {
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
            delete pRilData;
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnVerifyPinDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;
    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    m_nPinRemain[PIN1_PUK1] = adapter.GetRemainCount();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
#ifdef AUTO_VERIFY_PIN_WITH_CP
        m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());
        if (uErrCode != RIL_E_SUCCESS) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLED);
        }
#else
        if (uErrCode == RIL_E_SUCCESS) {
            if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_RECOVERY) {
                m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
                RilLogI("%s::%s() Automatic Verify PIN Done",
                    m_pSimService->m_szSvcName, __FUNCTION__);
                m_pSimService->DoGetSimStatus(pMsg);
            } else if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING) {
                const char *pszPIN = NULL;
                Message *pCurrMsg = m_pSimService->GetCurrentMsg();
                if (pCurrMsg != NULL) {
                    VerifyPIN *rildata = (VerifyPIN *)pCurrMsg->GetRequestData();
                    pszPIN = rildata->GetPIN();
                }
                m_pSimAutoVerifyPinHandler->SavePin(pszPIN);
                m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
            } else {
                LOGE("Current auto pin state: %d is abnormal.",
                        m_pSimAutoVerifyPinHandler->m_nAutoPinState);
                m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
            }
        } else { // uErrCode != RIL_E_SUCCESS
            if (m_pSimAutoVerifyPinHandler->m_nAutoPinState !=
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_RECOVERY) {
                LOGE("auto state is not recovery, send request complete message");
                m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
            } else {
                LOGE("Auto verifying pin is failed, so re-send get sim status request");
                m_pSimService->DoGetSimStatus(pMsg);
            }
            m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
        }
#endif
        if (pRilData != NULL) delete pRilData;
    }
    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoVerifyPin2(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    VerifyPIN *rildata = (VerifyPIN *)pMsg->GetRequestData();
    const char *pszPIN = rildata->GetPIN();
    const char *pszAID = rildata->GetAID();
    if (pszPIN != NULL && 0 < strlen(pszPIN)) {
        if(IsValidPinPuk(SIM_PIN, pszPIN) == FALSE) {
            SendPasswordInvalid(SIM_PIN, PIN2_PUK2);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimVerifyPin(PIN2_PUK2, pszPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_PIN2_DONE);
        }
    } else {
        //Empty PIN2 is to get remain count
        LOGV("Empty PIN2, Response Remain Count(%d)", m_nPinRemain[PIN2_PUK2]);
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimPinPukResponse(m_nPinRemain[PIN2_PUK2]);
        if (pRilData == NULL) {
            m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        } else {
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
            delete pRilData;
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnVerifyPin2Done(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());
    m_nPinRemain[PIN2_PUK2] = adapter.GetRemainCount();
    UINT uErrCode = adapter.GetErrorCode();
    SimDataBuilder builder;
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
        if (uErrCode == RIL_E_SUCCESS) {
            // PIN2 Verification for FDN enable/disable
            if (m_pEnableFdnData != NULL) {
                if(m_pSimService->SendRequest(m_pEnableFdnData, TIMEOUT_SIM_DEFAULT
                            , MSG_SIM_SET_FACILITY_LOCK_DONE) == -1) {
                    m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
                }
                // delete m_pEnableFdnData;
                m_pEnableFdnData = NULL;
            } else {
                m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                        pRilData->GetDataLength());
            }
        } else {
            // PIN2 is incorrect for FDN operation
            if (m_pEnableFdnData != NULL) {
                m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                        pRilData->GetDataLength());
                delete m_pEnableFdnData;
                m_pEnableFdnData = NULL;
            } else {
                m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                        pRilData->GetDataLength());
            }
        }

        if (NULL != pRilData) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoVerifyPuk(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    VerifyPUK *rildata = (VerifyPUK *)pMsg->GetRequestData();
    const char *pszPUK = rildata->GetPUK();
    const char *pszNewPIN = rildata->GetNewPIN();
    const char *pszAID = rildata->GetAID();
    if (pszPUK != NULL && 0 < strlen(pszPUK) && pszNewPIN != NULL && 0 < strlen(pszNewPIN)) {
        if (IsValidPinPuk(SIM_PUK, pszPUK) == FALSE ||
            IsValidPinPuk(SIM_PIN, pszNewPIN) == FALSE) {
            SendPasswordInvalid(SIM_PUK, PIN1_PUK1);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimVerifyPuk(PIN1_PUK1, pszPUK, pszNewPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_PUK_DONE);
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING);
        }
    } else {
        //Empty PUK is to get remain count
        LOGV("Empty PUK, Response Remain Count(%d)", m_nPukRemain[PIN1_PUK1]);
        SimDataBuilder builder;
        const RilData *pRilData =
        builder.BuildSimPinPukResponse(m_nPukRemain[PIN1_PUK1]);
        if (pRilData == NULL) {
            m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        } else {
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
            pRilData->GetDataLength());
            delete pRilData;
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnVerifyPukDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;
    ProtocolSimVerifyPukAdapter adapter(pMsg->GetModemData());
    m_nPukRemain[PIN1_PUK1] = adapter.GetRemainCount();
    UINT uErrCode = adapter.GetErrorCode();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
#ifdef AUTO_VERIFY_PIN_WITH_CP
        if (uErrCode == RIL_E_SUCCESS) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
        } else {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLED);
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
        }
#else
        if (uErrCode == RIL_E_SUCCESS) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
            const char *pszNewPIN = NULL;
            Message *pCurrMsg = m_pSimService->GetCurrentMsg();
            if (pCurrMsg != NULL) {
                VerifyPUK *rildata = (VerifyPUK *)pCurrMsg->GetRequestData();
                pszNewPIN = rildata->GetNewPIN();
            }
            m_pSimAutoVerifyPinHandler->SavePin(pszNewPIN);
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        } else {
            m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
        }
#endif
        if (pRilData != NULL) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoVerifyPuk2(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;

    VerifyPUK *rildata = (VerifyPUK *)pMsg->GetRequestData();
    const char *pszPUK = rildata->GetPUK();
    const char *pszNewPIN = rildata->GetNewPIN();
    const char *pszAID = rildata->GetAID();
    if (pszPUK != NULL && 0 < strlen(pszPUK) && pszNewPIN != NULL && 0 < strlen(pszNewPIN)) {
        if (IsValidPinPuk(SIM_PUK, pszPUK) == FALSE ||
            IsValidPinPuk(SIM_PIN, pszNewPIN) == FALSE) {
            SendPasswordInvalid(SIM_PUK, PIN2_PUK2);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimVerifyPuk(PIN2_PUK2, pszPUK, pszNewPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_PUK2_DONE);
        }
    } else {
        //Empty PUK2 is to get remain count
        LOGV("Empty PUK2, Response Remain Count(%d)", m_nPukRemain[PIN2_PUK2]);
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimPinPukResponse(m_nPukRemain[PIN2_PUK2]);
        if (pRilData == NULL) {
            m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
        } else {
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
            delete pRilData;
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnVerifyPuk2Done(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;

    ProtocolSimVerifyPukAdapter adapter(pMsg->GetModemData());
    m_nPukRemain[PIN2_PUK2] = adapter.GetRemainCount();
    UINT uErrCode = adapter.GetErrorCode();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
        if (uErrCode==RIL_E_SUCCESS)
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        else
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());

        if (pRilData != NULL) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoChangePin(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    ChangePIN *rildata = (ChangePIN *)pMsg->GetRequestData();
    const char *pszOldPIN = rildata->GetOldPIN();
    const char *pszNewPIN = rildata->GetNewPIN();
    const char *pszAID = rildata->GetAID();
    if (pszOldPIN != NULL && 0 < strlen(pszOldPIN) && pszNewPIN != NULL && 0 < strlen(pszNewPIN)) {
        if (IsValidPinPuk(SIM_PIN, pszOldPIN) == FALSE ||
            IsValidPinPuk(SIM_PIN, pszNewPIN) == FALSE) {
            SendPasswordInvalid(SIM_PIN, PIN1_PUK1);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimChangePin(
                        PIN1_PUK1, pszOldPIN, pszNewPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_CHANGE_PIN_DONE);
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLING);
        }
    } else {
        SendPasswordInvalid(SIM_PIN, PIN1_PUK1);
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnChangePinDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;
    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    m_nPinRemain[PIN1_PUK1] = adapter.GetRemainCount();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
#ifdef AUTO_VERIFY_PIN_WITH_CP
        m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                pRilData->GetDataLength());
        if (uErrCode != RIL_E_SUCCESS) {
            m_pSimAutoVerifyPinHandler->SetAutoPinState(
                    SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
        }
#else
        if (uErrCode == RIL_E_SUCCESS) {
            const char *pszNewPIN = NULL;
            Message *pCurrMsg = m_pSimService->GetCurrentMsg();
            if (pCurrMsg != NULL) {
                ChangePIN *rildata = (ChangePIN *)pCurrMsg->GetRequestData();
                pszNewPIN = rildata->GetNewPIN();
            }
            m_pSimAutoVerifyPinHandler->SavePin(pszNewPIN);
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        } else {
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());
        }
        m_pSimAutoVerifyPinHandler->SetAutoPinState(
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED);
#endif
        if (pRilData != NULL) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoChangePin2(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;
    ChangePIN *rildata = (ChangePIN *)pMsg->GetRequestData();
    const char *pszOldPIN = rildata->GetOldPIN();
    const char *pszNewPIN = rildata->GetNewPIN();
    const char *pszAID = rildata->GetAID();

    if (pszOldPIN != NULL && 0 < strlen(pszOldPIN) && pszNewPIN != NULL && 0 < strlen(pszNewPIN)) {
        if (IsValidPinPuk(SIM_PIN, pszOldPIN) == FALSE ||
            IsValidPinPuk(SIM_PIN, pszNewPIN) == FALSE) {
            SendPasswordInvalid(SIM_PIN, PIN2_PUK2);
        } else {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimChangePin(PIN2_PUK2, pszOldPIN, pszNewPIN, pszAID);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_CHANGE_PIN2_DONE);
        }
    } else {
        SendPasswordInvalid(SIM_PIN, PIN2_PUK2);
    }
    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnChangePin2Done(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;
    ProtocolSimVerifyPinAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    m_nPinRemain[PIN2_PUK2] = adapter.GetRemainCount();
    const RilData *pRilData = builder.BuildSimPinPukResponse(adapter.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
        if (uErrCode==RIL_E_SUCCESS)
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        else
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());

        if (pRilData != NULL) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

int SimLockHandler::DoVerifyNetworkLock(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = 0;

    // check last saved sim status and decide FAC type.
    int nFac = -1; // PN or PU or PP or PC
    V2_0::RIL_CardStatus status = m_pSimService->GetRilCardStatus();
    for (int i = 0; i < status.num_applications; i++) {
        if(status.applications[i].app_state == RIL_APPSTATE_SUBSCRIPTION_PERSO) {
            if (status.applications[i].perso_substate == RIL_PERSOSUBSTATE_SIM_NETWORK) {
                nFac = FacilityLock::FAC_PN;
                LOGV("FAC type is FacilityLock::FAC_PN");
                break;
            } else if (status.applications[i].perso_substate == RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET) {
                nFac = FacilityLock::FAC_PU;
                LOGV("FAC type is FacilityLock::FAC_PU");
                break;
            } else if (status.applications[i].perso_substate == RIL_PERSOSUBSTATE_SIM_CORPORATE) {
                nFac = FacilityLock::FAC_PP;
                LOGV("FAC type is FacilityLock::FAC_PP");
                break;
            } else if (status.applications[i].perso_substate == RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER) {
                nFac = FacilityLock::FAC_PC;
                LOGV("FAC type is FacilityLock::FAC_PC");
                break;
            }
        }
    }

    if (nFac == -1) {
        RilLogE("%s(): FAC type is invalid", __FUNCTION__);
    } else {
        VerifyNetLock *rildata = (VerifyNetLock *)pMsg->GetRequestData();
        const char *pszCode = rildata->GetDepersonalCode();
        if (pszCode != NULL && 0 < strlen(pszCode)) {
            ProtocolSimBuilder builder;
            ModemData *pModemData = builder.BuildSimVerifyNetworkLock(nFac, pszCode, 0, NULL);
            nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT,
                    MSG_SIM_VERIFY_NETWORK_LOCK_DONE);
        }
    }

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

int SimLockHandler::OnVerifyNetworkLockDone(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    SimDataBuilder builder;

    ProtocolSimVerifyNetLockAdapter adapterSimVerifyNetLock(pMsg->GetModemData());
    UINT uErrCode = adapterSimVerifyNetLock.GetErrorCode();
    const RilData *pRilData = builder.BuildSimNetworkLockResponse(
                adapterSimVerifyNetLock.GetRemainCount());
    RilLogV("remain count : %d", adapterSimVerifyNetLock.GetRemainCount());
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
        if (uErrCode == RIL_E_SUCCESS)
            m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(),
                    pRilData->GetDataLength());
        else
            m_pSimService->OnRequestComplete(uErrCode, pRilData->GetData(),
                    pRilData->GetDataLength());

        if (pRilData != NULL) delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

BOOL SimLockHandler::IsValidPinPuk(int nSimPinPuk, const char *pszPinPuk)
{
    if (pszPinPuk == NULL) return false;

    if ((nSimPinPuk == SIM_PIN && (strlen(pszPinPuk) < 4 || 8 < strlen(pszPinPuk)))
            || (nSimPinPuk == SIM_PUK && strlen(pszPinPuk) < 8)) {
        return FALSE;
    }

    for (int i = 0; i < (int)strlen(pszPinPuk); i++) {
        if (pszPinPuk[i] < '0' || '9' < pszPinPuk[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

int SimLockHandler::SendPasswordInvalid(int nSimPinPuk, int nPinPukIndex)
{
    ENTER_FUNC();

    int nRemainCount = -1;
    if (nSimPinPuk == SIM_PIN) nRemainCount = m_nPinRemain[nPinPukIndex];
    else if (nSimPinPuk == SIM_PUK) nRemainCount = m_nPukRemain[nPinPukIndex];

    SimDataBuilder builder;
    const RilData *pRilData = builder.BuildSimPinPukResponse(nRemainCount);
    if (pRilData == NULL) {
        m_pSimService->OnRequestComplete(RIL_E_NO_MEMORY);
    } else {
        m_pSimService->OnRequestComplete(RIL_E_PASSWORD_INCORRECT, pRilData->GetData(),
                pRilData->GetDataLength());
        delete pRilData;
    }

    LEAVE_FUNC();
    return 0;
}

void SimLockHandler::SetPinPukRemain(int nSimPinPuk, int nPinPukIndex, int nRemainCount)
{
    if (nSimPinPuk == SIM_PIN) m_nPinRemain[nPinPukIndex] = nRemainCount;
    else if (nSimPinPuk == SIM_PUK) m_nPukRemain[nPinPukIndex] = nRemainCount;
}

void SimLockHandler::InitializePinPuk()
{
    m_nPinRemain[PIN1_PUK1] = -1;
    m_nPinRemain[PIN2_PUK2] = -1;
    m_nPukRemain[PIN1_PUK1] = -1;
    m_nPukRemain[PIN2_PUK2] = -1;
}

int SimLockHandler::GetPinState()
{
    V2_0::RIL_CardStatus rilCardStatus = m_pSimService->GetRilCardStatus();
    int app_index = 0;
    if (rilCardStatus.applications[app_index].pin1_replaced == 0) {
        return rilCardStatus.applications[app_index].pin1;
    } else {
        return rilCardStatus.universal_pin_state;
    }
}
