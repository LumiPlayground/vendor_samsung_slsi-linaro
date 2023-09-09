/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __STK_PARSER_H__
#define __STK_PARSER_H__

#include "types.h"
#include "thread.h"
#include "stkmodule.h"

#include <queue>

class CMutex;
class Message;
class StkService;
class Comprehension;

class StkDataParser : public Thread {
public:
    StkDataParser(StkService *pStkService, int nRilSocketID = 0);
    virtual ~StkDataParser();

    // Not Used Yet
    virtual int OnCreate();
    // Not Used Yet
    virtual void OnStart();
    virtual void OnDestroy();

    bool IsStarted() { return m_bStarted; }

    int Initialize();
    int Finalize();
    virtual void Run();

protected:
    // Pipe
    int CreateCommandPipe();

    // Queue
    int Enqueue(const Message *pMsg);
    Message *Dequeue();
    void ClearQueue();

    int Process(Message *pMsg);
    int ProcessRequest(Message *pMsg);
    int ProcessModemData(int nMsgId, ModemData *pModem);

    // Request
    int ProcessEnvelopeCommand(Message *pMsg);
    int ProcessTerminalResponse(Message *pMsg);
    int ProcessEnvelopeStatus(RequestData *pReq);

    // Response
    int ProcessProactiveCommand(ModemData *pModem);
    int ProcessEnvelopeCommandDone(ModemData *pModem);
    int ProcessTerminalResponseDone(ModemData *pModem);
    int ProcessEnvelopeStatusDone(ModemData *pModem);

    int HandleCommand(Comprehension *pComprehension);

private:
    char m_szTAG[32];
    StkService *m_pStkService;
    friend class StkService;
    friend class CStkModule;
    CMutex *m_pMutex;
    bool m_bWait = false;
    bool m_bUserOper = false;
    Comprehension *m_pComprehension = NULL;

    // Command Pipe to be Read/Written to Queue
    int m_nCmdRead;
    int m_nCmdWritten;

    // STD Queue
    std::queue<Message *> m_cmdQueue;

    int m_nAccumlatedSendDataLenInBuffer;

    SetupEventList m_tSetupEventList;
};
#endif  // __STK_MODULE_H__
