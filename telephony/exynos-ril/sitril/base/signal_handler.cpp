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
 * signal_handler.cpp
 *
 *  Created on: 2015. 01. 21.
 *      Author: jhdaniel.kim
 */
#include <signal.h>
#include "signal_handler.h"
#include "rilapplication.h"
#include "rillog.h"

#include "rilcontext.h"
#include <librilutils/systemproperty.h>
#include "reset_util.h"

#ifndef PROP_VALUE_MAX
#define PROP_VALUE_MAX 92
#endif

#ifndef PROPERTY_NAME_LEN
#define PROPERTY_NAME_LEN   (40)
#endif

#define INVALID_PIN_LEN (-1)
#define INVALID_PIN_STR ""

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

 IMPLEMENT_MODULE_TAG(SignalMonitor, SignalHandler)

SignalMonitor* gpThis;
void signal_handler(int signo);
void sigaction_handler(int signum, siginfo_t *siginfo, void *context);

int register_signal_handler()
{
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    //signal(SIGPIPE, SIG_IGN);
    signal(SIGPIPE, signal_handler);
    return 0;
}

void unregister_signal_handler()
{
    signal(SIGUSR1, SIG_DFL);
    signal(SIGUSR2, SIG_DFL);

    signal(SIGPIPE, SIG_DFL);
}

void signal_handler(int signo)
{
    gpThis->HandleSignal(signo);
}

void sigaction_handler(int signum, siginfo_t *siginfo, void *context)
{
    gpThis->HandleSigActionHandler(signum, siginfo, context);
}

SignalMonitor::SignalMonitor(RilApplication *pApp)
{
    m_pRilApp = pApp;
    for (int i = E_RIL_SIGILL; i < E_RIL_SIGMAX; i++) {
        memset(&m_oldActList[i], 0x00, sizeof(struct sigaction));
    }
 }

SignalMonitor::~SignalMonitor()
{
    for (int i = E_RIL_SIGILL; i < E_RIL_SIGMAX; i++) {
        memset(&m_oldActList[i], 0x00, sizeof(struct sigaction));
    }
}

int SignalMonitor::Start()
{
    gpThis = this;
    register_signal_handler();

    // for RIL crash
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigemptyset (&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
    sigact.sa_sigaction = sigaction_handler;

    sigaction(SIGILL, &sigact, &m_oldActList[E_RIL_SIGILL]);
    sigaction(SIGABRT, &sigact, &m_oldActList[E_RIL_SIGABRT]);
    sigaction(SIGBUS, &sigact, &m_oldActList[E_RIL_SIGBUS]);
    sigaction(SIGFPE, &sigact, &m_oldActList[E_RIL_SIGFPE]);
    sigaction(SIGSEGV, &sigact, &m_oldActList[E_RIL_SIGSEGV]);
    sigaction(SIGSTKFLT, &sigact, &m_oldActList[E_RIL_SIGSTKFLT]);
    sigaction(SIGSYS, &sigact, &m_oldActList[E_RIL_SIGSYS]);

    return 0;
}

int SignalMonitor::Stop()
{
    unregister_signal_handler();

    // for RIL crash
    sigaction(SIGILL, &m_oldActList[E_RIL_SIGILL], NULL);
    sigaction(SIGABRT, &m_oldActList[E_RIL_SIGABRT], NULL);
    sigaction(SIGBUS, &m_oldActList[E_RIL_SIGBUS], NULL);
    sigaction(SIGFPE, &m_oldActList[E_RIL_SIGFPE], NULL);
    sigaction(SIGSEGV, &m_oldActList[E_RIL_SIGSEGV], NULL);
    sigaction(SIGSTKFLT, &m_oldActList[E_RIL_SIGSTKFLT], NULL);
    sigaction(SIGSYS, &m_oldActList[E_RIL_SIGSYS], NULL);

    return 0;
}

static int getHandlerIndex(int signum)
{
    int index = E_RIL_SIGMAX;
    switch(signum) {
    case SIGILL:    index = 0; break;
    case SIGABRT:   index = 1; break;
    case SIGBUS:    index = 2; break;
    case SIGFPE:    index = 3; break;
    case SIGSEGV:   index = 4; break;
    case SIGSTKFLT: index = 5; break;
    case SIGSYS:    index = 6; break;
    }

    return index;
}

int SignalMonitor::HandleSigActionHandler(int signum, siginfo_t *siginfo, void *context)
{
    switch(signum) {
    case SIGILL:
    case SIGABRT:
    case SIGBUS:
    case SIGFPE:
    case SIGSEGV:
    case SIGSTKFLT:
    case SIGSYS:
        {
            HandleRILCrash();

            struct sigaction oldAct;
            sigaction(signum, NULL, &oldAct);

            int index = getHandlerIndex(signum);
            if ((oldAct.sa_flags & SA_SIGINFO) == SA_SIGINFO) {
                if (E_RIL_SIGILL <= index && index < E_RIL_SIGMAX) {
                    // This operation is for android tombstone.
                    // After RIL saved encrypted data to system property
                    // excute saved sa_signaction.
                    if (m_oldActList[index].sa_sigaction != NULL) {
                        (m_oldActList[index].sa_sigaction)(signum, siginfo, context);
                    } else {
                        RilLogE("%s() : sa_sigaction is NULL", __FUNCTION__);
                        exit(0);
                    }
                } else {
                    RilLogE("%s() : Invalid Handler Index (%d)", __FUNCTION__, index);
                    exit(0);
                }
            } else {
                if (m_oldActList[index].sa_handler != NULL) {
                    (m_oldActList[index].sa_handler)(signum);
                } else {
                    RilLogE("%s() : sa_handler is NULL", __FUNCTION__);
                    exit(0);
                }
            }
        }
        break;
    }
    return 0;
}

int SignalMonitor::HandleSignal(int signo)
{
    int nActNo = 0;
    RilLogI("%s() : signo(%d)", __FUNCTION__, signo);

    switch(signo)
    {
    case SIGUSR1:
        RilLogV("[SIGNAL_HANDLER] SIGUSR1 reserved for dedicated usage");
        HandleSignalReserved();
        break;
    case SIGUSR2:
        RilLogV("[SIGNAL_HANDLER] SIGUSR2 used for dev");
        HandleSignalDevAction(nActNo);
        break;
    case SIGPIPE:
        RilLogE("[SIGNAL_HANDLER] SIGPIPE is caught");
        RilLogE("Restart RIL process for sync status between CP and RIL");
        RilErrorReset("SIGPIPE");
        break;
    default:
        RilLogE("[SIGNAL_HANDLER] Signal(%d) is caught", signo);
        break;
    }

    return 0;
}

int SignalMonitor::HandleRILCrash()
{
    RilLogI("%s() [<--", __FUNCTION__);

#ifdef AUTO_VERIFY_PIN_WITH_CP
    RilContext *pRilContext = NULL;
    std::string strEncryptedPin= "";
    int nPinLen = 0;
    char propertyName[PROPERTY_NAME_LEN] = {0};
    for (int nSocket = RIL_SOCKET_1; nSocket < SIM_COUNT; nSocket++) {
        pRilContext = m_pRilApp->GetRilContext((RIL_SOCKET_ID)nSocket);
        RilProperty *property = pRilContext->GetProperty();
        if (property != NULL) {
            nPinLen = property->GetInt(RIL_CONTEXT_SIM_PIN_LEN, INVALID_PIN_LEN);
            strEncryptedPin = property->GetString(RIL_CONTEXT_SIM_ENCRYPTED_PIN, INVALID_PIN_STR);

            if (INVALID_PIN_LEN == nPinLen || 0 == strEncryptedPin.length()) {
                continue;
            }

            snprintf(propertyName, (PROPERTY_NAME_LEN - 1), VENDOR_RIL_PIN_LEN, nSocket);
            SystemProperty::Set((const char*)propertyName, nPinLen);

            snprintf(propertyName, (PROPERTY_NAME_LEN - 1), VENDOR_RIL_ENCRYPTED_PIN, nSocket);
            SystemProperty::Set((const char*)propertyName, (const char*)strEncryptedPin.c_str());
        } else {
            RilLogE("[%s] property is NULL", __FUNCTION__);
        }
    }

#else
    for (int nSocket = RIL_SOCKET_1; nSocket < SIM_COUNT; nSocket++) {
        char propertyName[PROPERTY_NAME_LEN] = {0, };
        snprintf(propertyName, (PROPERTY_NAME_LEN - 1), PROP_AUTO_VERIFY_PIN, nSocket);
        std::string EncryptedPin = SystemProperty::Get(propertyName, "");
        if (EncryptedPin.length() != 0) {
            snprintf(propertyName, (PROPERTY_NAME_LEN - 1),
                AUTO_PIN_STATE_ENABLED_AND_RIL_CRASH, nSocket);
            SystemProperty::Set((const char*)propertyName, 1);
        }
    }
#endif

    RilLogI("%s() [-->", __FUNCTION__);
    return 0;
}

int SignalMonitor::HandleSignalReserved()
{
    return 0;
}

int SignalMonitor::HandleSignalDevAction(int dev_id)
{
    const int ID_RILLOG_CAT_CHANGED = 1;
    switch(dev_id)
    {
    case ID_RILLOG_CAT_CHANGED:
        // re-load the current log policy
        CRilLog::InitRilLog();
        break;
    }

    return 0;
}
