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
 * protocolstkadapterlegacy.cpp
 *
 *  Created on: 2014. 10. 6.
 *      Author: mox
 */


#include "protocolstkadapterlegacy.h"
#include "rillog.h"
#include "sitdef.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

#define SWAP16(val)                 ( (((val) << 8) & 0xFF00) | (((val) >> 8) & 0x00FF) )
#define TAG_ADDRESS_LEN             (32)
#define TAG_COMMON_LEN              (2) // for BER_TLV's TAG + LENGTH
#define MAX_PROACTIVE_COMMAND_LEN   (256)
/**
 * ProtocolStkEnvelopeCommandAdapterLegacy
 */
void ProtocolStkEnvelopeCommandAdapterLegacy::Init()
{
    m_nEnvelopeCmdLength = 0;
    m_pEnvelopeCmd = NULL;

    if (m_pModemData != NULL) {
        sit_stk_send_stk_envelope_cmd_rsp *pData = (sit_stk_send_stk_envelope_cmd_rsp *) m_pModemData->GetRawData();
        if (pData != NULL)
        {
            m_nEnvelopeCmdLength = pData->envelope_rsp_len;
            m_pEnvelopeCmd = pData->envelope_rsp;
        }
    }
}

/**
 * ProtocolStkTerminalRspAdapterLegacy
 */
void ProtocolStkTerminalRspAdapterLegacy::Init()
{
    nSW1 = -1;
    nSW2 = -1;

    if (m_pModemData != NULL) {
        sit_stk_send_stk_terminal_rsp_rsp *pData = (sit_stk_send_stk_terminal_rsp_rsp *) m_pModemData->GetRawData();
        if (pData != NULL)
        {
            nSW1 = (int) pData->sw1;
            nSW2 = (int) pData->sw2;
            RilLogV("ProtocolStkTerminalRspAdapterLegacy::%s() SW1:0x%02X, SW2:0x%02X", __FUNCTION__, nSW1, nSW2);
        }
    }
}

/**
 * ProtocolStkEnvelopeStatusAdapterLegacy
 */
void ProtocolStkEnvelopeStatusAdapterLegacy::Init()
{
    nSW1 = -1;
    nSW2 = -1;
    m_nEnvelopeRspLength = 0;
    m_pEnvelopeRsp = NULL;

    if (m_pModemData != NULL) {
        sit_stk_send_stk_envelope_with_status_rsp *pData = (sit_stk_send_stk_envelope_with_status_rsp *) m_pModemData->GetRawData();
        if (pData != NULL)
        {
            nSW1 = (int) pData->sw1;
            nSW2 = (int) pData->sw2;
            m_nEnvelopeRspLength = (int) pData->envelope_rsp_len;
            m_pEnvelopeRsp = pData->envelope_rsp;
        }
    }
}

/**
 * ProtocolStkProactiveCommandAdapterLegacy
 */
void ProtocolStkProactiveCommandAdapterLegacy::Init()
{
    RilLogI("StkProactiveCmdAdapterLegacy::%s() [<-- ", __FUNCTION__);

    m_nProactiveCmdLength = 0;
    m_pProactiveCmd = NULL;

    m_nCount = 0;
    memset(m_arEFID, 0, sizeof(UINT)*MAX_EFID_COUNT);
    m_nAidLen = 0;
    memset(m_acAID, 0, MAX_SIM_AID_LEN);

    if (m_pModemData == NULL) {
        RilLogE("StkProactiveCmdAdapterLegacy::%s() m_pModemData == NULL", __FUNCTION__);
        return;
    }

    sit_stk_stk_proactive_cmd_ind *pData = (sit_stk_stk_proactive_cmd_ind *) m_pModemData->GetRawData();
    if (pData == NULL) {
        RilLogE("StkProactiveCmdAdapterLegacy::%s() pData != NULL", __FUNCTION__);
        return;
    }

    m_nProactiveCmdLength = (pData->proactive_cmd_len > MAX_PROACTIVE_COMMAND_LEN) ?
            MAX_PROACTIVE_COMMAND_LEN : pData->proactive_cmd_len;
    m_pProactiveCmd = pData->proactive_cmd;

    BER_TLV *pTlv = (BER_TLV *) m_pProactiveCmd;
    if (pTlv && pTlv->cTag == PROACTIVE_CMD_BER_TAG && pTlv->cLength < m_nProactiveCmdLength) {
        RilLogV("StkProactiveCmdAdapterLegacy::%s() PROACTIVE_CMD_BER_TAG", __FUNCTION__);

        CMPH_CMD_DETAIL *ptCmdDetail = NULL;
        CMPH_DEVICE_ID *ptDevID = NULL;
        CMPH_RESULT *ptResult = NULL;
        CMPH_DURATION *ptDuration = NULL;
        CMPH_ADDRESS *ptAddress = NULL;
        CMPH_ALPHA_ID *ptAlpha = NULL;
        CMPH_TEXT_STRING *ptTextString = NULL;
        CMPH_FILE_LIST *ptFileList = NULL;
        CMPH_AID *ptAID = NULL;

        int nOffset = TAG_COMMON_LEN;    // for BER_TLV's TAG + LENGTH
        TAG_COMMON *ptTagCommon = NULL;
        if (m_nProactiveCmdLength > nOffset) {
            ptTagCommon = (TAG_COMMON *) &m_pProactiveCmd[nOffset];
        }

        // Parsing each TLV
        while (ptTagCommon && ptTagCommon->cLength > 0) {
            switch ((ptTagCommon->cTag & CAT_CMPH_TAG_MASK)) {
            case CAT_CMPH_TAG_CMD_DETAIL:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Command Detail(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptCmdDetail = (CMPH_CMD_DETAIL *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_DEVICE_ID:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Device Identifier(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptDevID = (CMPH_DEVICE_ID *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_RESULT:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Result(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptResult = (CMPH_RESULT *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_DURATION:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Duration(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptDuration = (CMPH_DURATION *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_ALPHA:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Alpha(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptAlpha = (CMPH_ALPHA_ID *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_ADDRESS:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Address(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptAddress = (CMPH_ADDRESS *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_TEXT_STRING:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: Text String(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptTextString = (CMPH_TEXT_STRING *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_FILE_LIST:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: File List(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
                ptFileList = (CMPH_FILE_LIST *) &m_pProactiveCmd[nOffset];
                break;
            case CAT_CMPH_TAG_AID:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() Tag: AID(0x%02X), Length:%d", __FUNCTION__, ptTagCommon->cTag, ptTagCommon->cLength);
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

        if (ptCmdDetail) {
            switch(ptCmdDetail->cType) {
            case CAT_CMD_TYPE_REFRESH:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_CMD_TYPE_REFRESH, Length:%d"
                        , __FUNCTION__, ptCmdDetail->common.cLength);

                if (ptFileList != NULL && ptFileList->cNumOfFiles > 0) {
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() File List, Length:%d, NumOfEFID:%d"
                            , __FUNCTION__, ptFileList->common.cLength, ptFileList->cNumOfFiles);
                    int nCandidateCount = (ptFileList->common.cLength - 1) / 2;   // 1 is number of EFID
                    //RilLogV("StkProactiveCmdAdapterLegacy::%s() File List, Candidate Count:%d", __FUNCTION__, nCandidateCount);
                    for (int i = 0; i < nCandidateCount && m_nCount < (int) ptFileList->cNumOfFiles; i++) {
                        WORD wEFID = SWAP16(ptFileList->awFiles[i]);
                        //RilLogV("StkProactiveCmdAdapterLegacy::%s() %d.Candidate EFID: 0x%04X", __FUNCTION__, i, wEFID);
                        // Except first
                        if (i > 0 && (wEFID & CAT_EFID_ROOT_MASK) == CAT_EFID_ROOT_PREFIX) {
                            wEFID = SWAP16(ptFileList->awFiles[i-1]);
                            m_arEFID[m_nCount] = wEFID;
                            m_nCount++;

                            RilLogV("StkProactiveCmdAdapterLegacy::%s() EFID: 0x%04X", __FUNCTION__, wEFID);
                        } else if (nCandidateCount == (i + 1)) { // Last one
                            m_arEFID[m_nCount] = wEFID;
                            m_nCount++;
                            RilLogV("StkProactiveCmdAdapterLegacy::%s() EFID: 0x%04X", __FUNCTION__, wEFID);
                        }
                    }
                }

                if (ptAID != NULL && ptAID->common.cLength > 0) {
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() AID, Length:%d",
                            __FUNCTION__, ptAID->common.cLength);
                    if (ptAID->common.cLength < MAX_SIM_AID_LEN) {
                        m_nAidLen = ptAID->common.cLength;
                    } else {
                        m_nAidLen = MAX_SIM_AID_LEN;
                    }
                    memcpy(m_acAID, ptAID->acAID, m_nAidLen);
                }
                break;
            case CAT_CMD_TYPE_DISPLAY_TEXT:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_CMD_TYPE_DISPLAY_TEXT, Length:%d", __FUNCTION__, ptCmdDetail->common.cLength);
                if(ptTextString)
                switch(ptTextString->cDataCodingScheme)
                {
                case CAT_TEXT_CODING_GSM7BIT:
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_TEXT_CODING_GSM7BIT", __FUNCTION__);
                    break;
                case CAT_TEXT_CODING_GSM8BIT:
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_TEXT_CODING_GSM8BIT", __FUNCTION__);
                    break;
                case CAT_TEXT_CODING_UCS2:
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_TEXT_CODING_UCS2", __FUNCTION__);
                    break;
                }
                break;
            case CAT_CMD_TYPE_SETUP_CALL:
                RilLogV("StkProactiveCmdAdapterLegacy::%s() CAT_CMD_TYPE_SETUP_CALL, Length:%d", __FUNCTION__, ptCmdDetail->common.cLength);
                if (ptDevID && ptDevID->cSrcId == CAT_DEVICE_ID_UICC
                        && ptDevID->cDstId == CAT_DEVICE_ID_NETWORK) {
                    RilLogV("StkProactiveCmdAdapterLegacy::%s() UICC -> Network", __FUNCTION__);
                    if(ptAddress && ptAddress->cTonNpiFlag == 1) {
                        RilLogV("StkProactiveCmdAdapterLegacy::%s() TON:0x%X", __FUNCTION__, ptAddress->cTon);
                        RilLogV("StkProactiveCmdAdapterLegacy::%s() NPI:0x%X", __FUNCTION__, ptAddress->cNpi);

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
                    else if ( ptAddress != NULL ){
                        RilLogV("StkProactiveCmdAdapterLegacy::%s() 0x%X, 0x%X, 0x%X",
                                __FUNCTION__, ptAddress->cTonNpiFlag, ptAddress->cTon,
                                ptAddress->cNpi);
                    }
                    else {
                        RilLogV("StkProactiveCmdAdapterLegacy : Address NULL");
                    }

                    //m_bStkCallSetup = TRUE;
                }
                else if(ptDevID) RilLogV("StkProactiveCmdAdapterLegacy::%s() 0x%02X -> 0x%02X, 0x%02X", __FUNCTION__, ptDevID->cSrcId, ptDevID->cDstId, ptCmdDetail->cQualifier);
                break;
            }
        }
    }

    RilLogI("StkProactiveCmdAdapterLegacy::%s() [--> ", __FUNCTION__);
}

/**
 * ProtocolStkSimRefreshAdapterLegacy
 */
void ProtocolStkSimRefreshAdapterLegacy::Init()
{
    m_nResult = 0;

    if (m_pModemData != NULL) {
        sit_stk_sim_refresh_ind *pData = (sit_stk_sim_refresh_ind *) m_pModemData->GetRawData();
        if (pData != NULL) {
            m_nResult = pData->result;
        }
    }
}

/**
 * ProtocolSsReturnResultAdapterLegacy
 */
void ProtocolSsReturnResultAdapterLegacy::Init()
{
    m_nReturnResultLength = 0;
    m_pReturnResult = NULL;

    if (m_pModemData != NULL) {
        sit_ss_return_result_ind *pData = (sit_ss_return_result_ind *) m_pModemData->GetRawData();
        if (pData != NULL)
        {
            m_nReturnResultLength = pData->return_result_len;
            m_pReturnResult = pData->return_result;
        }
    }
}

/**
 * ProtocolStkCcAlphaNtfAdapterLegacy
 */
int ProtocolStkCcAlphaNtfAdapterLegacy::GetAlphaLength() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_stk_cc_alpha_notify_ind *data = (sit_stk_cc_alpha_notify_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_STK_CC_ALPHA_NOTIFY)
            ret = data->alpha_len;
    }

    return ret;
}

BYTE *ProtocolStkCcAlphaNtfAdapterLegacy::GetAlpha() const
{
    BYTE *pRet = NULL;
    if (m_pModemData != NULL) {
        sit_stk_cc_alpha_notify_ind *data = (sit_stk_cc_alpha_notify_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_STK_CC_ALPHA_NOTIFY)
            pRet = data->alpha_buf;
    }

    return pRet;
}
