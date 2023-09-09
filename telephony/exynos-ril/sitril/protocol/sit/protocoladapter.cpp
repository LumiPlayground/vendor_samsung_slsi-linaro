/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "networkutils.h"
#include "protocoladapter.h"
#include "sitdef.h"
#include "rillog.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ConvertProtocolErrorCodeToRilErrorCode
 * @param errorCode enum of Protocol Error Code
 * @return enum of RIL Error Code
 */
int ConvertProtocolErrorCodeToRilErrorCode(int errorCode)
{
    switch(errorCode) {
        case RCM_E_SUCCESS:
            return RIL_E_SUCCESS;
        case RCM_E_RADIO_NOT_AVAILABLE:
            return RIL_E_RADIO_NOT_AVAILABLE;
        case RCM_E_GENERIC_FAILURE:
            return RIL_E_GENERIC_FAILURE;
        case RCM_E_PASSWORD_INCORRECT:
            return RIL_E_PASSWORD_INCORRECT;
        case RCM_E_SIM_PIN2:
            return RIL_E_SIM_PIN2;
        case RCM_E_SIM_PUK2:
            return RIL_E_SIM_PUK2;
        case RCM_E_REQUEST_NOT_SUPPORTED:
        case RCM_E_UNDEFINED_CMD:
            return RIL_E_REQUEST_NOT_SUPPORTED;
        case RCM_E_CANCELLED:
            return RIL_E_CANCELLED;
        case RCM_E_OP_NOT_ALLOWED_DURING_VOICE_CALL:
            return RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL;
        case RCM_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW:
            return RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW;
        case RCM_E_SMS_SEND_FAIL_RETRY:
            return RIL_E_SMS_SEND_FAIL_RETRY;
        case RCM_E_SIM_ABSENT:
            return RIL_E_SIM_ABSENT;
        case RCM_E_SUBSCRIPTION_NOT_AVAILABLE:
            return RIL_E_SUBSCRIPTION_NOT_AVAILABLE;
        case RCM_E_MODE_NOT_SUPPORTED:
            return RIL_E_MODE_NOT_SUPPORTED;
        case RCM_E_FDN_CHECK_FAILURE:
            return RIL_E_FDN_CHECK_FAILURE;
        case RCM_E_ILLEGAL_SIM_OR_ME:
            return RIL_E_ILLEGAL_SIM_OR_ME;
        case RCM_E_MISSING_RESOURCE:
            return RIL_E_MISSING_RESOURCE;
        case RCM_E_NO_SUCH_ELEMENT:
            return RIL_E_NO_SUCH_ELEMENT;
        case RCM_E_SIM_MEMORY_FULL:
            return RIL_E_SIM_FULL;
        case RCM_E_SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED:
            return RIL_E_SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED;
        case RCM_E_ACCESS_BARRED:
            return RIL_E_ACCESS_BARRED;
        case RCM_E_BLOCKED_DUE_TO_CALL:
            return RIL_E_BLOCKED_DUE_TO_CALL;
        case RCM_E_RF_HARDWARE_ISSUE:
            return RIL_E_RF_HARDWARE_ISSUE;
        case RCM_E_NO_RF_CALIBRATION_INFO:
            return RIL_E_NO_RF_CALIBRATION_INFO;
        case RCM_E_MODEM_ERR:
            return RIL_E_MODEM_ERR;
        case RCM_E_INVALID_SIM_STATE:
            return RIL_E_INVALID_SIM_STATE;
        case RCM_E_PB_TOO_LONG_DATA:
        case RCM_E_SIM_PIN_REQ:
        case RCM_E_SIM_PUK_REQ:
        case RCM_E_OP_NOT_ALLOWED_DURING_PLMN_SEARCH:
        case RCM_E_NOT_ALLOWED_DURING_PLMN_SEARCH:
        default:
            break;
    }
    return RIL_E_GENERIC_FAILURE;
}

//////////////////////////////////////////////////////////////////////////////
// ProtocolBaseAdapter implement
//////////////////////////////////////////////////////////////////////////////
UINT ProtocolBaseAdapter::GetType() const {
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL/* && m_pModemData->GetLength() >= sizeof(RCM_HEADER)*/) {
            return (UINT)(rcmdata->type & 0x000000FF);
        }
    }
    return 0;
}

UINT ProtocolBaseAdapter::GetId() const {
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL/* && m_pModemData->GetLength() >= sizeof(RCM_HEADER)*/) {
            return (UINT)(rcmdata->id & 0x0000FFFF);
        }
    }
    return 0;
}

UINT ProtocolBaseAdapter::GetLength() const {
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL/* && m_pModemData->GetLength() >= sizeof(RCM_HEADER)*/) {
            return (UINT)(rcmdata->length & 0x0000FFFF);
        }
    }
    return 0;
}

UINT ProtocolBaseAdapter::GetToken() const
{
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL/* && m_pModemData->GetLength() >= sizeof(RCM_HEADER)*/) {
            if (rcmdata->type == RCM_TYPE_REQUEST || rcmdata->type == RCM_TYPE_RESPONSE)
                return rcmdata->ext.req.token;
        }
    }
    return TOKEN_INVALID;
}

const char *ProtocolBaseAdapter::GetParameter() const
{
    const char *rawdata = (m_pModemData != NULL ? m_pModemData->GetRawData() : NULL);
    if (rawdata != NULL && m_pModemData->GetLength() > (int)sizeof(RCM_HEADER)) {
        return rawdata + sizeof(RCM_HEADER);
    }
    return NULL;
}

UINT ProtocolBaseAdapter::GetParameterLength() const
{
    if (m_pModemData->GetLength() > (int)sizeof(RCM_HEADER)) {
        return (UINT)(m_pModemData->GetLength() - (int)sizeof(RCM_HEADER));
    }
    return 0;
}

bool ProtocolBaseAdapter::IsRequest() const
{
    return GetType() == RCM_TYPE_REQUEST;
}

bool ProtocolBaseAdapter::IsResponse() const
{
    UINT type = GetType();
    return (type == RCM_TYPE_RESPONSE || type == RCM_TYPE_INDICATION);
}

bool ProtocolBaseAdapter::IsUnsolicitedResponse() const
{
    return GetType() == RCM_TYPE_INDICATION;
}

bool ProtocolBaseAdapter::IsSolicitedResponse() const
{
    return GetType() == RCM_TYPE_RESPONSE;
}

//////////////////////////////////////////////////////////////////////////////
// RCMRespAdapter
//////////////////////////////////////////////////////////////////////////////
UINT ProtocolRespAdapter::GetErrorCode() const
{
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL && m_pModemData->GetLength() >= (int)sizeof(RCM_HEADER)) {
            if (rcmdata->type == RCM_TYPE_RESPONSE)
                return ConvertProtocolErrorCodeToRilErrorCode((UINT)(rcmdata->ext.rsp.error & 0xFF));
        }
    }
    // default error code
    return RIL_E_GENERIC_FAILURE;
}

//////////////////////////////////////////////////////////////////////////////
// RCMIndAdapter implement
//////////////////////////////////////////////////////////////////////////////
const char *ProtocolIndAdapter::GetParameter() const
{
    const char *rawdata = (m_pModemData != NULL ? m_pModemData->GetRawData() : NULL);
    if (rawdata != NULL && m_pModemData->GetLength() > (int)sizeof(RCM_IND_HEADER)) {
        return rawdata + sizeof(RCM_IND_HEADER);
    }
    return NULL;
}

UINT ProtocolIndAdapter::GetParameterLength() const
{
    if (m_pModemData->GetLength() > (int)sizeof(RCM_IND_HEADER)) {
        return (UINT)(m_pModemData->GetLength() - (int)sizeof(RCM_IND_HEADER));
    }
    return 0;
}
