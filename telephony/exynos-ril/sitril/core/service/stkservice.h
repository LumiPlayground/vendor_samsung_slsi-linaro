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
 * simservice.cpp
 *
 *  Created on: 2018.03.09.
 *      Author: MOX
 */

#ifndef __STK_SERVICE_H__
#define __STK_SERVICE_H__

#include "service.h"

#define    TIMEOUT_STK_DEFAULT        30000

class Message;
class RilContext;
class ModemData;
class StkEfidList;
class StkDataParser;

class StkService : public Service
{
public:
    StkService(RilContext* pRilContext);
    StkService(const StkService &) = delete;
    StkService& operator=(StkService const&) = delete;
    virtual ~StkService();

protected:
    virtual int OnCreate(RilContext *pRilContext);

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);

    virtual void OnSimStatusChanged(int cardState, int appState);

public:
    virtual bool IsDefaultApnConnecting();

protected:
    virtual int OnSimHotSwap(BOOL bRemoval);

    virtual int DoStkIsRunning(Message *pMsg);

    virtual int DoSendEvelopeCommand(Message *pMsg);
    virtual int OnSendEvelopeCommandDone(Message *pMsg);

    virtual int DoSendTerminalResponse(Message *pMsg);
    virtual int OnSendTerminalResponseDone(Message *pMsg);

    virtual int DoSendEnvelopeStatus(Message *pMsg);
    virtual int OnSendEnvelopeStatusDone(Message *pMsg);

    virtual int DoStkHandleCallSetupReqFromSim(Message *pMsg);
    virtual int OnStkHandleCallSetupReqFromSimDone(Message *pMsg);

    virtual int OnProactiveCommand(Message *pMsg);
    virtual int OnSimRefresh(Message *pMsg);

    virtual int OnSsReturnResult(Message *pMsg);
    virtual int OnSessionEnd(Message *pMsg);

    virtual int OnStkCcAlphaNtf(Message *pMsg);

    virtual bool IsPossibleToPassInRadioOffState(int request_id);

protected:
    BOOL m_bStkRunning;

    Message *m_pProactiveCmd;
    StkEfidList *m_pStkEfidList;

    int m_nCardState;

    // For STK Parser
    static const BOOL STK_PARSER_ENABLED = true;
    StkDataParser *m_pStkDataParser;
    friend class StkDataParser;

    virtual int SendStkParser(Message *pMsg);
};

#endif /*__STK_SERVICE_H__*/
