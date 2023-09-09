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
 * protocolstkadapterjson.h
 *
 *  Created on: 2021. 08. 12.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_STK_ADAPTER_JSON_H__
#define __PROTOCOL_STK_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolstkadapterinterface.h"

class ProtocolStkEnvelopeCommandAdapterJson : public ProtocolStkEnvelopeCommandAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolStkEnvelopeCommandAdapterJson(const ModemData *pModemData);
    ProtocolStkEnvelopeCommandAdapterJson(const ProtocolStkEnvelopeCommandAdapterJson&) = delete;
    ProtocolStkEnvelopeCommandAdapterJson& operator=(ProtocolStkEnvelopeCommandAdapterJson const&) = delete;
    virtual ~ProtocolStkEnvelopeCommandAdapterJson() {
        if (m_pEnvelopeCmd) {
            delete[] m_pEnvelopeCmd;
            m_pEnvelopeCmd = NULL;
        }
    }

  private:
    BYTE *m_pEnvelopeCmd;

    void Init();

  public:
    int GetEnvelopeCmdLength() const;
    BYTE *GetEnvelopeCommand() const {
        return m_pEnvelopeCmd;
    }
    void SetEnvelopeCommand();
};

class ProtocolStkEnvelopeStatusAdapterJson : public ProtocolStkEnvelopeStatusAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolStkEnvelopeStatusAdapterJson(const ModemData *pModemData);
    ProtocolStkEnvelopeStatusAdapterJson(const ProtocolStkEnvelopeStatusAdapterJson&) = delete;
    ProtocolStkEnvelopeStatusAdapterJson& operator=(ProtocolStkEnvelopeStatusAdapterJson const&) = delete;
    virtual ~ProtocolStkEnvelopeStatusAdapterJson() {
        if (m_pEnvelopeRsp) {
            delete[] m_pEnvelopeRsp;
            m_pEnvelopeRsp = NULL;
        }
    }

  private:
    BYTE *m_pEnvelopeRsp;

  public:
    int GetSW1() const;
    int GetSW2() const;
    int GetEnvelopeRspLength() const;
    BYTE *GetEnvelopeResponse() const {
        return m_pEnvelopeRsp;
    }

  private:
    void SetEnvelopeResponse();
    void Init();
};

class ProtocolStkProactiveCommandAdapterJson : public ProtocolStkProactiveCommandAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolStkProactiveCommandAdapterJson(const ModemData *pModemData);
    ProtocolStkProactiveCommandAdapterJson(const ProtocolStkProactiveCommandAdapterJson&) = delete;
    ProtocolStkProactiveCommandAdapterJson& operator=(ProtocolStkProactiveCommandAdapterJson const&) = delete;
    virtual ~ProtocolStkProactiveCommandAdapterJson() {
        if (m_pProactiveCmd) {
            delete[] m_pProactiveCmd;
            m_pProactiveCmd = NULL;
        }
    }

  private:
    static const BYTE PROACTIVE_CMD_BER_TAG = 0xD0;
    static const BYTE CAT_CMPH_TAG_MASK = 0x7F;
    static const WORD CAT_EFID_ROOT_MASK = 0xFF00;
    static const WORD CAT_EFID_ROOT_PREFIX = 0x3F00;
    static const int MAX_EFID_COUNT = 255;

    unsigned int m_nProactiveCmdLength;
    BYTE *m_pProactiveCmd;

    int m_nCount;
    UINT m_arEFID[MAX_EFID_COUNT];
    int m_nAidLen;
    BYTE m_acAID[MAX_SIM_AID_LEN];  // RID 5byte + PIX 11 byte

  public:
    int GetProactiveCmdLength() const {
        return m_nProactiveCmdLength;
    }
    BYTE *GetProactiveCommand() const {
        return m_pProactiveCmd;
    }

    int GetEfidCount(void) {
        return m_nCount;
    }
    UINT GetEFID(int nIndex) {
        return (nIndex < m_nCount) ? m_arEFID[nIndex] : 0;
    }

    int GetAidLength(void) {
        return m_nAidLen;
    }
    BYTE *GetAID(void) {
        return m_acAID;
    }

  private:
    void SetProactiveCommand();
    void DecodeProactiveCommand();
    void Init();
};

class ProtocolStkSimRefreshAdapterJson : public ProtocolStkSimRefreshAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolStkSimRefreshAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolStkSimRefreshAdapterJson() = default;

  public:
    int GetResult() const;
};

class ProtocolSsReturnResultAdapterJson : public ProtocolSsReturnResultAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolSsReturnResultAdapterJson(const ModemData *pModemData);
    ProtocolSsReturnResultAdapterJson(const ProtocolSsReturnResultAdapterJson&) = delete;
    ProtocolSsReturnResultAdapterJson& operator=(ProtocolSsReturnResultAdapterJson const&) = delete;
    virtual ~ProtocolSsReturnResultAdapterJson() {
        if (m_pReturnResult) {
            delete[] m_pReturnResult;
            m_pReturnResult = NULL;
        }
    }

  private:
    unsigned int m_nReturnResultLength;
    BYTE *m_pReturnResult;

    void SetReturnResult();

  public:
    int GetReturnResultLength() const {
        return m_nReturnResultLength;
    }
    BYTE *GetReturnResult() const {
        return m_pReturnResult;
    }

  private:
    void Init();
};

#endif /* __PROTOCOL_STK_ADAPTER_JSON_H__ */
