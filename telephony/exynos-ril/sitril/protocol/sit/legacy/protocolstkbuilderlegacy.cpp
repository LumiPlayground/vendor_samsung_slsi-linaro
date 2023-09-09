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
 * protocolstkbuilderlegacy.cpp
 *
 *  Created on: 2014. 10. 6.
 *      Author: MOX
 */
#include "protocolstkbuilderlegacy.h"
#include "rillog.h"
#include "modemdata.h"
#include <slsi/ril_oem.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ModemData *ProtocolStkBuilderLegacy::BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd)
{
    sit_stk_send_stk_envelope_cmd_req req;
    int length = sizeof(req);
    memset(&req, 0, length);

    if(nLength==0 || pEnvelopeCmd==NULL) return NULL;

    InitRequestHeader(&req.hdr, SIT_SEND_STK_ENVELOPE_CMD, length);
    req.envelope_cmd_len = (WORD) (nLength < MAX_SIM_IO_DATA_LEN) ? nLength : MAX_SIM_IO_DATA_LEN;
    memcpy(req.envelope_cmd, pEnvelopeCmd, req.envelope_cmd_len);
    RilLogV("ProtocolStkBuilderLegacy::%s() Data Length: %d", __FUNCTION__, req.envelope_cmd_len);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolStkBuilderLegacy::BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp)
{
    RilLogV("ProtocolStkBuilderLegacy::%s()", __FUNCTION__);

    sit_stk_send_stk_terminal_rsp_req req;
    int length = sizeof(req);
    memset(&req, 0, length);

    if(nLength==0 || pTerminalRsp==NULL) return NULL;

    // Make Limitation for Over Length
    nLength = (nLength>MAX_SIM_IO_DATA_LEN)? MAX_SIM_IO_DATA_LEN: nLength;
    InitRequestHeader(&req.hdr, SIT_SEND_STK_TERMINAL_RSP, length);
    //req.terminal_rsp_len = (WORD) HexString2Value(req.terminal_rsp, (char *) pTerminalRsp);
    req.terminal_rsp_len = (WORD) nLength;
    memcpy(req.terminal_rsp, pTerminalRsp, nLength);
    RilLogV("ProtocolStkBuilderLegacy::%s() Data Length: %d", __FUNCTION__, req.terminal_rsp_len);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolStkBuilderLegacy::BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus)
{
    sit_stk_send_stk_envelope_with_status_req req;
    int length = sizeof(req);
    memset(&req, 0, length);

    if(nLength==0 || pEnvelopeStatus==NULL) return NULL;

    InitRequestHeader(&req.hdr, SIT_SEND_STK_ENVELOPE_WITH_STATUS, length);
    req.envelope_cmd_len = (WORD) (nLength < MAX_SIM_IO_DATA_LEN) ? nLength : MAX_SIM_IO_DATA_LEN;
    memcpy(req.envelope_cmd, pEnvelopeStatus, req.envelope_cmd_len);
    RilLogV("ProtocolStkBuilderLegacy::%s() Data Length: %d", __FUNCTION__, req.envelope_cmd_len);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolStkBuilderLegacy::BuildStkCallSetup(int nUserOper)
{
    sit_stk_call_setup_req req;
    int length = sizeof(req);
    memset(&req, 0, length);

    InitRequestHeader(&req.hdr, SIT_STK_CALL_SETUP, length);
    req.user_operation = ((BYTE) nUserOper==SIT_STK_CALL_SETUP_ACCEPT)? SIT_STK_CALL_SETUP_ACCEPT : SIT_STK_CALL_SETUP_REJECT;
    RilLogV("ProtocolStkBuilderLegacy::%s() Data Length: %d", __FUNCTION__, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolStkBuilderLegacy::BuildOemSatWifiRequest(int msgId, BYTE *pData, int nDataLength)
{
    int nSitMsg;
    switch(msgId)
    {
    case RIL_REQUEST_OEM_WIFI_STATUS_RESPONSE: nSitMsg = SIT_SEND_STK_WIFI_STATUS_REPORT; break;
    case RIL_REQUEST_OEM_WIFI_CHANNEL_STATUS_REPORT: nSitMsg = SIT_SEND_STK_TCP_STATUS_EVENT; break;
    case RIL_REQUEST_OEM_WIFI_OPEN_CHANNEL_RESPONSE: nSitMsg = SIT_SEND_STK_WIFI_OPEN_CHANNEL_RSP; break;
    case RIL_REQUEST_OEM_WIFI_SEND_DATA_RESPONSE: nSitMsg = SIT_STK_SEND_WIFI_DATA_RESPONSE; break;
    case RIL_REQUEST_OEM_WIFI_RECEIVE_DATA_RESPONSE: nSitMsg = SIT_STK_SEND_BIP_DATA_TO_SIM; break;
    case RIL_REQUEST_OEM_WIFI_CLOSE_CHANNEL_RESPONSE: nSitMsg = SIT_STK_SEND_CLOSE_CHANNEL_RESPONSE; break;
    default: return NULL;
    }

    BYTE *pBuffer = new BYTE[sizeof(null_data_format)+nDataLength];
    null_data_format *pReq = (null_data_format *) pBuffer;
    int length = sizeof(null_data_format) + nDataLength;
    InitRequestHeader(&pReq->hdr, nSitMsg, length);
    memcpy(pBuffer+sizeof(null_data_format), pData, nDataLength);

    ModemData *pModemData = new ModemData((char *)pReq, length);
    delete [] pBuffer;

    return pModemData;
}
