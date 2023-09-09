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
 * protocolstkbuilderjson.cpp
 *
 *  Created on: 2021. 08. 12.
 *      Author: aryaman.2019
 */
#include "protocolstkbuilderjson.h"
#include "protocolutils.h"
#include "stkjsondef.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ModemData *ProtocolStkBuilderJson::BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd)
{
    if (nLength == 0 || pEnvelopeCmd == NULL) return NULL;

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_STK, __CLASS_FUNC__, nLength, MAX_SIM_IO_DATA_LEN);

    Json::Value jsonReqData;
    for (int i = 0; i < nLength; i++) {
        jsonReqData[ns_stk_send_stk_envelope_cmd_req::key_envelope_cmd].append((int) pEnvelopeCmd[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_STK_ENVELOPE_CMD);
}

ModemData *ProtocolStkBuilderJson::BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp)
{
    if (nLength == 0 || pTerminalRsp == NULL) return NULL;

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_STK, __CLASS_FUNC__, nLength, MAX_SIM_IO_DATA_LEN);

    Json::Value jsonReqData;
    for (int i = 0; i < nLength; i++) {
        jsonReqData[ns_stk_send_stk_terminal_rsp_req::key_terminal_rsp].append((int) pTerminalRsp[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_STK_TERMINAL_RSP);
}

ModemData *ProtocolStkBuilderJson::BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus)
{
    if (nLength == 0 || pEnvelopeStatus == NULL) return NULL;

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_STK, __CLASS_FUNC__, nLength, MAX_SIM_IO_DATA_LEN);

    Json::Value jsonReqData;
    for (int i = 0; i < nLength; i++) {
          jsonReqData[ns_stk_send_stk_envelope_with_status_req::key_envelope_cmd].append((int) pEnvelopeStatus[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_STK_ENVELOPE_WITH_STATUS);
}

ModemData *ProtocolStkBuilderJson::BuildStkCallSetup(int nUserOper)
{
    Json::Value jsonReqData;
    jsonReqData[ns_stk_call_setup_req::key_user_operation] = (nUserOper == SIT_STK_CALL_SETUP_ACCEPT) ?
        SIT_STK_CALL_SETUP_ACCEPT : SIT_STK_CALL_SETUP_REJECT;

    return GetJsonReqModemData(jsonReqData, SIT_STK_CALL_SETUP);
}

ModemData *ProtocolStkBuilderJson::BuildOemSatWifiRequest(int /* msgId */, BYTE * /* pData */, int /* nDataLength */) {
    return NULL;
}
