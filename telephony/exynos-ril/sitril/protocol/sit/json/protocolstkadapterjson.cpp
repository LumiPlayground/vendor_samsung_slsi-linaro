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
 * protocolstkadapterjson.cpp
 *
 *  Created on: 2021. 08.12.
 *      Author: aryaman.2019
 */


#include "protocolstkadapterjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "stkjsondef.h"
#include "sitdef.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define SWAP16(val)                 ( (((val) << 8) & 0xFF00) | (((val) >> 8) & 0x00FF) )
#define TAG_ADDRESS_LEN             (32)
#define TAG_COMMON_LEN              (2) // for BER_TLV's TAG + LENGTH
#define MAX_PROACTIVE_COMMAND_LEN   (256)
/**
 * ProtocolStkEnvelopeCommandAdapterJson
 */
ProtocolStkEnvelopeCommandAdapterJson::ProtocolStkEnvelopeCommandAdapterJson(const ModemData *pModemData)
    : ProtocolStkEnvelopeCommandAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolStkEnvelopeCommandAdapterJson::Init() {
    ParseJsonPayload(SIT_SEND_STK_ENVELOPE_CMD);
    m_pEnvelopeCmd = NULL;
    SetEnvelopeCommand();
}

void ProtocolStkEnvelopeCommandAdapterJson::SetEnvelopeCommand() {
    if (m_bParsingSuccess) {
        int nEnvelopeCmdLen = GetEnvelopeCmdLength();
        m_pEnvelopeCmd = new BYTE[nEnvelopeCmdLen];
        for (int i = 0; i < nEnvelopeCmdLen; i++) {
            m_pEnvelopeCmd[i] = (BYTE) m_jsonData[ns_stk_send_stk_envelope_cmd_rsp::key_envelope_rsp][i].asInt();
        }
    }
}

int ProtocolStkEnvelopeCommandAdapterJson::GetEnvelopeCmdLength() const {
    unsigned int nEnvelopeCmdLen = 0;
    if (m_bParsingSuccess) {
        nEnvelopeCmdLen = m_jsonData[ns_stk_send_stk_envelope_cmd_rsp::key_envelope_rsp].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_STK, __CLASS_FUNC__, nEnvelopeCmdLen, MAX_SIM_IO_DATA_LEN);
    }
    return nEnvelopeCmdLen;
}

/**
 * ProtocolStkEnvelopeStatusAdapterJson
 */
ProtocolStkEnvelopeStatusAdapterJson::ProtocolStkEnvelopeStatusAdapterJson(const ModemData *pModemData)
    : ProtocolStkEnvelopeStatusAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolStkEnvelopeStatusAdapterJson::Init() {
    ParseJsonPayload(SIT_SEND_STK_ENVELOPE_WITH_STATUS);
    m_pEnvelopeRsp = NULL;
    SetEnvelopeResponse();
}

void ProtocolStkEnvelopeStatusAdapterJson::SetEnvelopeResponse() {
    if(m_bParsingSuccess) {
        int nEnvelopeRspLen = GetEnvelopeRspLength();
        m_pEnvelopeRsp = new BYTE[nEnvelopeRspLen];
        for(int i = 0; i < nEnvelopeRspLen; i++) {
            m_pEnvelopeRsp[i] = (BYTE) m_jsonData[ns_stk_send_stk_envelope_with_status_rsp::key_envelope_rsp][i].asInt();
        }
    }
}

int ProtocolStkEnvelopeStatusAdapterJson::GetEnvelopeRspLength() const {
    unsigned int nEnvelopeRspLen = 0;
    if(m_bParsingSuccess) {
        nEnvelopeRspLen = m_jsonData[ns_stk_send_stk_envelope_with_status_rsp::key_envelope_rsp].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_STK, __CLASS_FUNC__, nEnvelopeRspLen, MAX_SIM_IO_DATA_LEN);
    }
    return nEnvelopeRspLen;
}

int ProtocolStkEnvelopeStatusAdapterJson::GetSW1() const {
    int sw1 = 0;
    if(m_bParsingSuccess) {
        sw1 = m_jsonData[ns_stk_send_stk_envelope_with_status_rsp::key_sw1].asInt();
    }
    return sw1;
}

int ProtocolStkEnvelopeStatusAdapterJson::GetSW2() const {
    int sw2 = 0;
    if(m_bParsingSuccess) {
        sw2 = m_jsonData[ns_stk_send_stk_envelope_with_status_rsp::key_sw2].asInt();
    }
    return sw2;
}

/**
 * ProtocolStkProactiveCommandAdapterJson
 */
ProtocolStkProactiveCommandAdapterJson::ProtocolStkProactiveCommandAdapterJson(const ModemData *pModemData)
    : ProtocolStkProactiveCommandAdapterInterface(), ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolStkProactiveCommandAdapterJson::Init() {
    m_nProactiveCmdLength = 0;
    m_pProactiveCmd = NULL;

    m_nCount = 0;
    memset(m_arEFID, 0, sizeof(UINT) * MAX_EFID_COUNT);
    m_nAidLen = 0;
    memset(m_acAID, 0, MAX_SIM_AID_LEN);

    ParseJsonPayload(SIT_IND_STK_PROACTIVE_COMMAND);
    SetProactiveCommand();
}

void ProtocolStkProactiveCommandAdapterJson::SetProactiveCommand() {
    if(!m_bParsingSuccess) {
        return;
    }
    m_nProactiveCmdLength = m_jsonData[ns_stk_proactive_cmd_ind::key_proactive_cmd].size();
    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_STK, __CLASS_FUNC__, m_nProactiveCmdLength, MAX_PROACTIVE_COMMAND_LEN);
    m_pProactiveCmd = new BYTE[m_nProactiveCmdLength];
    for(unsigned int i = 0; i < m_nProactiveCmdLength; i++) {
        m_pProactiveCmd[i] = m_jsonData[ns_stk_proactive_cmd_ind::key_proactive_cmd][i].asInt();
    }
    DecodeProactiveCommand();
}

void ProtocolStkProactiveCommandAdapterJson::DecodeProactiveCommand() {
    if(!m_bParsingSuccess) {
        return;
    }
    BER_TLV *pTlv = (BER_TLV *) m_pProactiveCmd;
    if (pTlv && pTlv->cTag == PROACTIVE_CMD_BER_TAG && pTlv->cLength < m_nProactiveCmdLength) {
        RilLogV("StkProactiveCmdAdapterJson::%s() PROACTIVE_CMD_BER_TAG", __FUNCTION__);
        CMPH_CMD_DETAIL *ptCmdDetail = NULL;
        CMPH_DEVICE_ID *ptDevID = NULL;
        CMPH_RESULT *ptResult = NULL;
        CMPH_DURATION *ptDuration = NULL;
        CMPH_ADDRESS *ptAddress = NULL;
        CMPH_ALPHA_ID *ptAlpha = NULL;
        CMPH_TEXT_STRING *ptTextString = NULL;
        CMPH_FILE_LIST *ptFileList = NULL;
        CMPH_AID *ptAID = NULL;

        unsigned int nOffset = TAG_COMMON_LEN;    // for BER_TLV's TAG + LENGTH
        TAG_COMMON *ptTagCommon = NULL;
        if (m_nProactiveCmdLength > nOffset) {
            ptTagCommon = (TAG_COMMON *) &m_pProactiveCmd[nOffset];
        }

        // Parsing each TLV
        while (ptTagCommon && ptTagCommon->cLength > 0) {
            switch((ptTagCommon->cTag & CAT_CMPH_TAG_MASK))
            {
                case CAT_CMPH_TAG_CMD_DETAIL:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Command Detail(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptCmdDetail = (CMPH_CMD_DETAIL *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_DEVICE_ID:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Device Identifier(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptDevID = (CMPH_DEVICE_ID *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_RESULT:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Result(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptResult = (CMPH_RESULT *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_DURATION:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Duration(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptDuration = (CMPH_DURATION *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_ALPHA:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Alpha(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptAlpha = (CMPH_ALPHA_ID *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_ADDRESS:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Address(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptAddress = (CMPH_ADDRESS *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_TEXT_STRING:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: Text String(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptTextString = (CMPH_TEXT_STRING *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_FILE_LIST:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: File List(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptFileList = (CMPH_FILE_LIST *) &m_pProactiveCmd[nOffset];
                    break;
                case CAT_CMPH_TAG_AID:
                    RilLogV("StkProactiveCmdAdapterJson::%s() Tag: AID(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                    ptAID = (CMPH_AID *) &m_pProactiveCmd[nOffset];
                    break;
            }

            nOffset += (TAG_COMMON_LEN + ptTagCommon->cLength);
            if (m_nProactiveCmdLength > nOffset) {
                ptTagCommon = (TAG_COMMON *) &m_pProactiveCmd[nOffset];
            } else {
                ptTagCommon = NULL;
            }
        }

        if(ptCmdDetail) {
            switch(ptCmdDetail->cType)
            {
                case CAT_CMD_TYPE_REFRESH:
                    RilLogV("StkProactiveCmdAdapterJson::%s() CAT_CMD_TYPE_REFRESH, Length:%d", __FUNCTION__, ptCmdDetail->common.cLength);

                    if(ptFileList != NULL && ptFileList->cNumOfFiles > 0) {
                        RilLogV("StkProactiveCmdAdapterJson::%s() File List, Length:%d, NumOfEFID:%d", __FUNCTION__, ptFileList->common.cLength, ptFileList->cNumOfFiles);
                        int nCandidateCount = (ptFileList->common.cLength - 1) / 2;   // 1 is number of EFID
                        // RilLogV("StkProactiveCmdAdapterJson::%s() File List, Candidate Count:%d", __FUNCTION__, nCandidateCount);
                        for(int i = 0; i < nCandidateCount && m_nCount < (int)ptFileList->cNumOfFiles; i++) {
                            WORD wEFID = SWAP16(ptFileList->awFiles[i]);
                            // RilLogV("StkProactiveCmdAdapterJson::%s() %d.Candidate EFID: 0x%04X", __FUNCTION__, i, wEFID);
                            // Except first
                            if(i > 0 && (wEFID & CAT_EFID_ROOT_MASK) == CAT_EFID_ROOT_PREFIX) {
                                wEFID = SWAP16(ptFileList->awFiles[i-1]);
                                m_arEFID[m_nCount] = wEFID;
                                m_nCount++;

                                RilLogV("StkProactiveCmdAdapterJson::%s() EFID: 0x%04X", __FUNCTION__, wEFID);
                            }
                            // Last one
                            else if(nCandidateCount == (i+1)) {
                                m_arEFID[m_nCount] = wEFID;
                                m_nCount++;

                                RilLogV("StkProactiveCmdAdapterJson::%s() EFID: 0x%04X", __FUNCTION__, wEFID);
                            }
                        }
                    }

                    if (ptAID != NULL && ptAID->common.cLength > 0) {
                        RilLogV("StkProactiveCmdAdapter::%s() AID, Length:%d", __FUNCTION__, ptAID->common.cLength);
                        if (ptAID->common.cLength < MAX_SIM_AID_LEN) {
                            m_nAidLen = ptAID->common.cLength;
                        } else {
                            m_nAidLen = MAX_SIM_AID_LEN;
                        }
                        memcpy(m_acAID, ptAID->acAID, m_nAidLen);
                    }
                    break;
                case CAT_CMD_TYPE_DISPLAY_TEXT:
                    RilLogV("StkProactiveCmdAdapterJson::%s() CAT_CMD_TYPE_DISPLAY_TEXT, Length:%d", __FUNCTION__, ptCmdDetail->common.cLength);
                    if(ptTextString)
                        switch(ptTextString->cDataCodingScheme)
                        {
                            case CAT_TEXT_CODING_GSM7BIT:
                                RilLogV("StkProactiveCmdAdapterJson::%s() CAT_TEXT_CODING_GSM7BIT", __FUNCTION__);
                                break;
                            case CAT_TEXT_CODING_GSM8BIT:
                                RilLogV("StkProactiveCmdAdapterJson::%s() CAT_TEXT_CODING_GSM8BIT", __FUNCTION__);
                                break;
                            case CAT_TEXT_CODING_UCS2:
                                RilLogV("StkProactiveCmdAdapterJson::%s() CAT_TEXT_CODING_UCS2", __FUNCTION__);
                                break;
                        }
                    break;
                case CAT_CMD_TYPE_SETUP_CALL:
                    RilLogV("StkProactiveCmdAdapterJson::%s() CAT_CMD_TYPE_SETUP_CALL, Length:%d", __FUNCTION__, ptCmdDetail->common.cLength);
                    if(ptDevID && ptDevID->cSrcId == CAT_DEVICE_ID_UICC && ptDevID->cDstId == CAT_DEVICE_ID_NETWORK) {
                        RilLogV("StkProactiveCmdAdapterJson::%s() UICC -> Network", __FUNCTION__);
                        if(ptAddress && ptAddress->cTonNpiFlag == 1) {
                            RilLogV("StkProactiveCmdAdapterJson::%s() TON:0x%X", __FUNCTION__, ptAddress->cTon);
                            RilLogV("StkProactiveCmdAdapterJson::%s() NPI:0x%X", __FUNCTION__, ptAddress->cNpi);

                            // Parsing Dailing Number to String
                            BOOL bParsing = TRUE;
                            int nDialNumLen = 0;
                            std::string strDialNumString = "";
                            if (ptAddress->common.cLength > TAG_ADDRESS_LEN) {
                                bParsing = FALSE;
                            }

                            for (nDialNumLen = 0; bParsing && nDialNumLen < ((ptAddress->common.cLength - 1)*2); nDialNumLen++) {
                                int idx = (nDialNumLen == 0) ? 0 : (nDialNumLen / 2);
                                unsigned char ch = ptAddress->acDialNumString[idx];
                                ch = (nDialNumLen == 0 ? ch : (nDialNumLen % 2 == 1 ? (ch >> 4) : ch)) & 0x0F;

                                if (/*ch>=0 &&*/ ch<=9) {
                                    strDialNumString.append(sizeof(char), ch + 0x30);
                                } else {
                                    switch(ch) {
                                    case 0x0A: strDialNumString.append(sizeof(char), '*'); break;
                                    case 0x0B: strDialNumString.append(sizeof(char), '#'); break;
                                    case 0x0C: strDialNumString.append(sizeof(char), 'p'); break;
                                    case 0x0D:
                                    case 0x0E: break;
                                    case 0x0F: bParsing = FALSE; break;
                                    }
                                }
                            }

                            RilLogV("StkProactiveCmdAdapterLegacy::%s() Dialing Number String:[%s]",
                                    __FUNCTION__, strDialNumString.c_str());
                        }
                        else if (ptAddress != NULL ) {
                            RilLogV("StkProactiveCmdAdapterJson::%s() 0x%X, 0x%X, 0x%X", __FUNCTION__, ptAddress->cTonNpiFlag, ptAddress->cTon, ptAddress->cNpi);
                        }
                        else {
                            RilLogV("StkProactiveCmdAdapterJson : Address NULL");
                        }

                        //m_bStkCallSetup = TRUE;
                    }
                    else if (ptDevID) RilLogV("StkProactiveCmdAdapterJson::%s() 0x%02X -> 0x%02X, 0x%02X", __FUNCTION__, ptDevID->cSrcId, ptDevID->cDstId, ptCmdDetail->cQualifier);
                    break;
            }
        }
    }
}

/**
 * ProtocolStkSimRefreshAdapterJson
 */
ProtocolStkSimRefreshAdapterJson::ProtocolStkSimRefreshAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SIM_REFRESH);
}

int ProtocolStkSimRefreshAdapterJson::GetResult() const {
    int nResult = 0;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_stk_sim_refresh_ind::key_refresh_result].asInt();
    }
    return nResult;
}

/**
 * ProtocolSsReturnResultAdapterJson
 */
ProtocolSsReturnResultAdapterJson::ProtocolSsReturnResultAdapterJson(const ModemData *pModemData)
    : ProtocolSsReturnResultAdapterInterface(), ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolSsReturnResultAdapterJson::Init() {
    m_nReturnResultLength = -1;
    m_pReturnResult = NULL;
    ParseJsonPayload(SIT_IND_SS_RETURN_RESULT);
    SetReturnResult();
}

void ProtocolSsReturnResultAdapterJson::SetReturnResult() {
    if (m_bParsingSuccess) {
        m_nReturnResultLength =
            m_jsonData[ns_stk_ss_return_result_ind::key_ret_result_list].size();

        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_STK, __CLASS_FUNC__, m_nReturnResultLength, MAX_SS_RETURN_RESULT);

        m_pReturnResult = new BYTE[m_nReturnResultLength];
        for (unsigned int i = 0; i < m_nReturnResultLength; i++) {
            m_pReturnResult[i] =
                (BYTE) m_jsonData[ns_stk_ss_return_result_ind::key_ret_result_list][i].asInt();
        }
    }
}
