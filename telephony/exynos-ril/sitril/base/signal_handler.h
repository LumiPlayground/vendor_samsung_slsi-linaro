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
 * signal_handler.h
 *
 *  Created on: 2015. 01. 21.
 *      Author: jhdaniel.kim
 */


#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include "base/basedef.h"

class RilApplication;

enum {
    E_RIL_SIGILL = 0,
    E_RIL_SIGABRT,
    E_RIL_SIGBUS,
    E_RIL_SIGFPE,
    E_RIL_SIGSEGV,
    E_RIL_SIGSTKFLT,
    E_RIL_SIGSYS,
    E_RIL_SIGMAX
} E_RIL_SIG_INDEX;

class SignalMonitor {
    DECLARE_MODULE_TAG()
public:

private:
    RilApplication *m_pRilApp;
    struct sigaction m_oldActList[E_RIL_SIGMAX];
public:
    int HandleSignal(int signo);
    int HandleSigActionHandler(int signum, siginfo_t *siginfo, void *context);
    int HandleSignalReserved();
    int HandleSignalDevAction(int dev_id);
    int HandleRILCrash();

public:
    explicit SignalMonitor(RilApplication *pApp);
    virtual ~SignalMonitor();

    int Start();
    int Stop();
};


#endif /* __SIGNAL_HANDLER_H__ */
