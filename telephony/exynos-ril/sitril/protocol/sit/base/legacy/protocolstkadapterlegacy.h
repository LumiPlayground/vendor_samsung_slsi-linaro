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
 * protocolstkadapterlegacy.h
 *
 *  Created on: 2014. 10. 6.
 *      Author: MOX
 */

#ifndef __PROTOCOL_STK_ADAPTER_LEGACY_H__
#define __PROTOCOL_STK_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolstkadapterinterface.h"

class ProtocolStkResponseAdapterLegacy : public ProtocolStkResponseAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolStkResponseAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkResponseAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolStkResponseAdapterLegacy() = default;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    const char *GetParameter() const {
        return ProtocolBaseAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }
};

class ProtocolStkIndAdapterLegacy : public ProtocolStkIndAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolStkIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkIndAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolStkIndAdapterLegacy() = default;

    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

class ProtocolStkEnvelopeCommandAdapterLegacy : public ProtocolStkEnvelopeCommandAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolStkEnvelopeCommandAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkEnvelopeCommandAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolStkEnvelopeCommandAdapterLegacy() = default;

  private:
    int m_nEnvelopeCmdLength;
    BYTE *m_pEnvelopeCmd;

    virtual void Init();

  public:
    int GetEnvelopeCmdLength() const {
        return m_nEnvelopeCmdLength;
    }
    BYTE *GetEnvelopeCommand() const {
        return m_pEnvelopeCmd;
    }
};

// not in use by service
class ProtocolStkTerminalRspAdapterLegacy : public ProtocolStkTerminalRspAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolStkTerminalRspAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkTerminalRspAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolStkTerminalRspAdapterLegacy() = default;

  private:
    int nSW1;
    int nSW2;

    virtual void Init();

  public:
    int GetSW1() const {
        return nSW1;
    }
    int GetSW2() const {
        return nSW2;
    }
};

class ProtocolStkEnvelopeStatusAdapterLegacy : public ProtocolStkEnvelopeStatusAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolStkEnvelopeStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkEnvelopeStatusAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolStkEnvelopeStatusAdapterLegacy() = default;

  private:
    int nSW1;
    int nSW2;
    int m_nEnvelopeRspLength;
    BYTE *m_pEnvelopeRsp;

    virtual void Init();

  public:
    int GetSW1() const {
        return nSW1;
    }
    int GetSW2() const {
        return nSW2;
    }
    int GetEnvelopeRspLength() const {
        return m_nEnvelopeRspLength;
    }
    BYTE *GetEnvelopeResponse() const {
        return m_pEnvelopeRsp;
    }
};

class ProtocolStkProactiveCommandAdapterLegacy : public ProtocolStkProactiveCommandAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolStkProactiveCommandAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkProactiveCommandAdapterInterface(), ProtocolIndAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolStkProactiveCommandAdapterLegacy() = default;

  private:
    virtual void Init();

    static const BYTE PROACTIVE_CMD_BER_TAG = 0xD0;
    static const BYTE CAT_CMPH_TAG_MASK = 0x7F;
    static const WORD CAT_EFID_ROOT_MASK = 0xFF00;
    static const WORD CAT_EFID_ROOT_PREFIX = 0x3F00;
    static const int MAX_EFID_COUNT = 255;

    int m_nProactiveCmdLength;
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
};

class ProtocolStkSimRefreshAdapterLegacy : public ProtocolStkSimRefreshAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolStkSimRefreshAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkSimRefreshAdapterInterface(), ProtocolIndAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolStkSimRefreshAdapterLegacy() = default;

  private:
    int m_nResult;

    virtual void Init();

  public:
    int GetResult() const {
        return m_nResult;
    }
};

class ProtocolSsReturnResultAdapterLegacy : public ProtocolSsReturnResultAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolSsReturnResultAdapterLegacy(const ModemData *pModemData)
        : ProtocolSsReturnResultAdapterInterface(), ProtocolIndAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolSsReturnResultAdapterLegacy() = default;

  private:
    int m_nReturnResultLength;
    BYTE *m_pReturnResult;

    virtual void Init();

  public:
    int GetReturnResultLength() const {
        return m_nReturnResultLength;
    }
    BYTE *GetReturnResult() const {
        return m_pReturnResult;
    }
};

class ProtocolStkCcAlphaNtfAdapterLegacy : public ProtocolStkCcAlphaNtfAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolStkCcAlphaNtfAdapterLegacy(const ModemData *pModemData)
        : ProtocolStkCcAlphaNtfAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolStkCcAlphaNtfAdapterLegacy() = default;
    int GetAlphaLength() const;
    BYTE *GetAlpha() const;
};
#endif /* __PROTOCOL_STK_ADAPTER_LEGACY_H__ */
