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
 * protocolstkadapter.h
 *
 *  Created on: 2021. 06. 17.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_STK_ADAPTER_H__

#include "protocolstkadapterinterface.h"

class ModemData;

class ProtocolStkResponseAdapter : public ProtocolStkResponseAdapterInterface {
private:
    ProtocolStkResponseAdapterInterface *protocolStkResponseAdapterInterface = NULL;

public:
    ProtocolStkResponseAdapter(const ModemData *pModemData);
    ProtocolStkResponseAdapter(const ProtocolStkResponseAdapter&) = delete;
    ProtocolStkResponseAdapter& operator=(ProtocolStkResponseAdapter const&) = delete;
    virtual ~ProtocolStkResponseAdapter();

    UINT GetErrorCode() const { return protocolStkResponseAdapterInterface->GetErrorCode(); }
    const char *GetParameter() const { return protocolStkResponseAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolStkResponseAdapterInterface->GetParameterLength(); }
};

class ProtocolStkIndAdapter : public ProtocolStkIndAdapterInterface {
private:
    ProtocolStkIndAdapterInterface *protocolStkIndAdapterInterface = NULL;

public:
    ProtocolStkIndAdapter(const ModemData *pModemData);
    ProtocolStkIndAdapter(const ProtocolStkIndAdapter&) = delete;
    ProtocolStkIndAdapter& operator=(ProtocolStkIndAdapter const&) = delete;
    virtual ~ProtocolStkIndAdapter();

    const char *GetParameter() const { return protocolStkIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolStkIndAdapterInterface->GetParameterLength(); }
};

class ProtocolStkEnvelopeCommandAdapter : public ProtocolStkEnvelopeCommandAdapterInterface {
private:
    ProtocolStkEnvelopeCommandAdapterInterface *protocolStkEnvelopeCommandAdapterInterface = NULL;

public:
    ProtocolStkEnvelopeCommandAdapter(const ModemData *pModemData);
    ProtocolStkEnvelopeCommandAdapter(const ProtocolStkEnvelopeCommandAdapter&) = delete;
    ProtocolStkEnvelopeCommandAdapter& operator=(ProtocolStkEnvelopeCommandAdapter const&) = delete;
    virtual ~ProtocolStkEnvelopeCommandAdapter();

public:
    int GetEnvelopeCmdLength() const { return protocolStkEnvelopeCommandAdapterInterface->GetEnvelopeCmdLength(); }
    BYTE *GetEnvelopeCommand() const { return protocolStkEnvelopeCommandAdapterInterface->GetEnvelopeCommand(); }
};

// not in use by service
class ProtocolStkTerminalRspAdapter : public ProtocolStkTerminalRspAdapterInterface {
private:
    ProtocolStkTerminalRspAdapterInterface *protocolStkTerminalRspAdapterInterface = NULL;

public:
    ProtocolStkTerminalRspAdapter(const ModemData *pModemData);
    ProtocolStkTerminalRspAdapter(const ProtocolStkTerminalRspAdapter&) = delete;
    ProtocolStkTerminalRspAdapter& operator=(ProtocolStkTerminalRspAdapter const&) = delete;
    virtual ~ProtocolStkTerminalRspAdapter();

public:
    int GetSW1() const  { return protocolStkTerminalRspAdapterInterface->GetSW1(); }
    int GetSW2() const  { return protocolStkTerminalRspAdapterInterface->GetSW2(); }
};

class ProtocolStkEnvelopeStatusAdapter : public ProtocolStkEnvelopeStatusAdapterInterface {
private:
    ProtocolStkEnvelopeStatusAdapterInterface *protocolStkEnvelopeStatusAdapterInterface = NULL;

public:
    ProtocolStkEnvelopeStatusAdapter(const ModemData *pModemData);
    ProtocolStkEnvelopeStatusAdapter(const ProtocolStkEnvelopeStatusAdapter&) = delete;
    ProtocolStkEnvelopeStatusAdapter& operator=(ProtocolStkEnvelopeStatusAdapter const&) = delete;
    virtual ~ProtocolStkEnvelopeStatusAdapter();

public:
    int GetSW1() const  { return protocolStkEnvelopeStatusAdapterInterface->GetSW1(); }
    int GetSW2() const  { return protocolStkEnvelopeStatusAdapterInterface->GetSW2(); }
    int GetEnvelopeRspLength() const { return protocolStkEnvelopeStatusAdapterInterface->GetEnvelopeRspLength(); }
    BYTE *GetEnvelopeResponse() const { return protocolStkEnvelopeStatusAdapterInterface->GetEnvelopeResponse(); }
};

class ProtocolStkProactiveCommandAdapter : public ProtocolStkProactiveCommandAdapterInterface {
private:
    ProtocolStkProactiveCommandAdapterInterface *protocolStkProactiveCommandAdapterInterface = NULL;

public:
    ProtocolStkProactiveCommandAdapter(const ModemData *pModemData);
    ProtocolStkProactiveCommandAdapter(const ProtocolStkProactiveCommandAdapter&) = delete;
    ProtocolStkProactiveCommandAdapter& operator=(ProtocolStkProactiveCommandAdapter const&) = delete;
    virtual ~ProtocolStkProactiveCommandAdapter();

    int GetProactiveCmdLength() const { return protocolStkProactiveCommandAdapterInterface->GetProactiveCmdLength(); }
    BYTE *GetProactiveCommand() const { return protocolStkProactiveCommandAdapterInterface->GetProactiveCommand(); }

    int GetEfidCount(void) { return protocolStkProactiveCommandAdapterInterface->GetEfidCount(); }
    UINT GetEFID(int nIndex) { return protocolStkProactiveCommandAdapterInterface->GetEFID(nIndex); }

    int GetAidLength(void) { return protocolStkProactiveCommandAdapterInterface->GetAidLength(); }
    BYTE *GetAID(void) { return protocolStkProactiveCommandAdapterInterface->GetAID(); }
};

class ProtocolStkSimRefreshAdapter : public ProtocolStkSimRefreshAdapterInterface {
private:
    ProtocolStkSimRefreshAdapterInterface *protocolStkSimRefreshAdapterInterface = NULL;

public:
    ProtocolStkSimRefreshAdapter(const ModemData *pModemData);
    ProtocolStkSimRefreshAdapter(const ProtocolStkSimRefreshAdapter&) = delete;
    ProtocolStkSimRefreshAdapter& operator=(ProtocolStkSimRefreshAdapter const&) = delete;
    virtual ~ProtocolStkSimRefreshAdapter();

public:
    int GetResult() const { return protocolStkSimRefreshAdapterInterface->GetResult(); }
};

class ProtocolSsReturnResultAdapter : public ProtocolSsReturnResultAdapterInterface {
private:
    ProtocolSsReturnResultAdapterInterface *protocolSsReturnResultAdapterInterface = NULL;

public:
    ProtocolSsReturnResultAdapter(const ModemData *pModemData);
    ProtocolSsReturnResultAdapter(const ProtocolSsReturnResultAdapter&) = delete;
    ProtocolSsReturnResultAdapter& operator=(ProtocolSsReturnResultAdapter const&) = delete;
    virtual ~ProtocolSsReturnResultAdapter();

public:
    int GetReturnResultLength() const { return protocolSsReturnResultAdapterInterface->GetReturnResultLength(); }
    BYTE *GetReturnResult() const { return protocolSsReturnResultAdapterInterface->GetReturnResult(); }
};

class ProtocolStkCcAlphaNtfAdapter : public ProtocolStkCcAlphaNtfAdapterInterface {
private:
    ProtocolStkCcAlphaNtfAdapterInterface *protocolStkCcAlphaNtfAdapterInterface = NULL;

public:
    ProtocolStkCcAlphaNtfAdapter(const ModemData *pModemData);
    ProtocolStkCcAlphaNtfAdapter(const ProtocolStkCcAlphaNtfAdapter&) = delete;
    ProtocolStkCcAlphaNtfAdapter& operator=(ProtocolStkCcAlphaNtfAdapter const&) = delete;
    virtual ~ProtocolStkCcAlphaNtfAdapter();

    int GetAlphaLength() const;
    BYTE* GetAlpha() const;
};
#endif /* __PROTOCOL_STK_ADAPTER_H__ */
