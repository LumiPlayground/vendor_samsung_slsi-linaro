/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "supplementaryservice.h"
#include "rillog.h"
#include "protocolcallbuilder.h"
#include "protocolcalladapter.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

SupplementaryService::SupplementaryService(RilContext *pRilContext)
    : Service(pRilContext, RIL_SERVICE_SUPPLEMENTARY)
{
    m_ussdUserInitiated = true;
    mCardState = RIL_CARDSTATE_ABSENT;
}

SupplementaryService::~SupplementaryService()
{
}

int SupplementaryService::OnCreate(RilContext * /* pRilContext */)
{
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);
    return 0;
}

BOOL SupplementaryService::OnHandleRequest(Message *pMsg)
{
    INT32 nRet = -1;
    if(NULL == pMsg)
    {
        return FALSE;
    }

    switch (pMsg->GetMsgId())
    {
    case MSG_SS_SEND_ENCODED_USSD:
        nRet = DoSendEncodedUssd();
        break;
    default:
        break;
    }

    if (0 == nRet)
        return TRUE;
    else
        return FALSE;
}

BOOL SupplementaryService::OnHandleSolicitedResponse(Message *pMsg)
{
    if(NULL == pMsg)
        return FALSE;

    switch (pMsg->GetMsgId())
    {
    default:// it should add its exception handling
        RilLogE("[SupplementaryService::%s] solicited response handler is not specified. error", __FUNCTION__);
        break;
    }

    return FALSE;
}

BOOL SupplementaryService::OnHandleUnsolicitedResponse(Message *pMsg)
{
    if(NULL == pMsg)
        return FALSE;

    switch (pMsg->GetMsgId())
    {
    default:
        RilLogE("[SupplementaryService::%s] unsolicited response handler is not specified. error", __FUNCTION__);
        break;
    }

    return TRUE;
}


BOOL SupplementaryService::OnHandleInternalMessage(Message* pMsg)
{
    //int nMsgId = 0;
    //char *pData = NULL;
    if (NULL == pMsg)
    {
        RilLogE("Received null internal message");
    }

    return TRUE;
}

void SupplementaryService::OnSimStatusChanged(int cardState, int appState)
{
    mCardState = cardState;
    RilLogV("[%s] Card state: %d, App state: %d", __FUNCTION__, cardState, appState);
}

INT32 SupplementaryService::DoSendEncodedUssd()
{
    RilLogI("[SupplementaryService] %s", __FUNCTION__);
    if (IsNullRequest(m_pCurReqMsg))
    {
        return -1;
    }

    StringsRequestData* pReq = (StringsRequestData*) m_pCurReqMsg->GetRequestData();

    RilLogV("[SupplementaryService] %s() Param1: %s", __FUNCTION__, pReq->GetString(0));
    BYTE dcs = (BYTE) strtol(pReq->GetString(0), NULL, 16);

    RilLogV("[SupplementaryService] %s() dcs: 0x%02X, Param2: %s", __FUNCTION__, dcs, pReq->GetString(1));
    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSendEncodedUssd(dcs, pReq->GetString(1), m_ussdUserInitiated);
    m_ussdUserInitiated = true;
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, MSG_SS_SEND_USSD_DONE) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL SupplementaryService::IsNullRequest(Message *pMsg)
{
    if (NULL == pMsg || NULL == pMsg->GetRequestData())
    {
        RilLogE("[SupplementaryService] %s, pMsg or RequestData is NULL!!", __FUNCTION__);
        return TRUE;
    }
    return FALSE;
}

bool SupplementaryService::IsPossibleToPassInRadioOffState(int request_id)
{
    switch (request_id) {
        default:
            return false;
    }
    return true;
}

/*
 *  Allowed RadioError on CardState::ABSENT is
 *   RadioError::NONE
 *   RadioError::INVALID_STATE
 *   RadioError::MODEM_ERR
 *  GeneralErrors on VTS-HIDL
 *   RadioError::RADIO_NOT_AVAILABLE (radio resetting)
 *   RadioError::REQUEST_NOT_SUPPORTED
 *   RadioError::CANCELLED
 *   RadioError::NO_MEMORY
 *   RadioError::INTERNAL_ERR
 *   RadioError::SYSTEM_ERR
 *   RadioError::SYSTEM_ERR
*/
int SupplementaryService::GetValidErrors(int errorCode)
{
    const static int allowErrors[] = {
        RIL_E_SUCCESS, RIL_E_RADIO_NOT_AVAILABLE, RIL_E_REQUEST_NOT_SUPPORTED, RIL_E_CANCELLED,
        RIL_E_NO_MEMORY, RIL_E_INTERNAL_ERR, RIL_E_SYSTEM_ERR, RIL_E_MODEM_ERR, RIL_E_INVALID_STATE};

    if (mCardState != RIL_CARDSTATE_ABSENT) return errorCode;

    int size = sizeof(allowErrors) / sizeof(allowErrors[0]);
    for (int i = 0 ; i < size ; i++)
    {
        if (errorCode == allowErrors[i])
        {
            return errorCode;
        }
    }
    return RIL_E_INTERNAL_ERR;
}

BOOL SupplementaryService::IsOperatorUsingUnknownServiceClass()
{
    std::string simOperator = GetSimOperatorNumeric();
    const char *simPlmn = simOperator.c_str();
    if(strncmp(simPlmn, "71610", 5) == 0)
    {
        return true;
    }
    return false;
}
