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
 * dmstateimpl.h
 *
 *  Created on: 2021. 9. 14.
 */

#ifndef __DM_STATE_IMPL_H__
#define __DM_STATE_IMPL_H__

#include "dmstatemachine.h"

class HostAgentManager;
class ModemProxy;
struct ModemStateLinstener;
struct OnBoardLogging;
struct SilentLogging;
struct SilentLoggingOnFactory;

struct DMStateBase : public DMState {
protected:
    DMStateMachine& mDsm;
    ModemProxy *mModemProxy;
    bool mEntered = false;
public:
    DMStateBase(DMStateMachine& dsm, ModemProxy *modemProxy);
    virtual ~DMStateBase()=default;
    //@override
    void enter() {
        mEntered = true;
        onEnter();
    }
    //@override
    void exit() {
        mEntered = false;
        onExit();
    }
protected:
    virtual void onEnter()=0;
    virtual void onExit()=0;
};

struct HostState : public DMStateBase {
protected:
    HostAgentManager *mHostAgentManager;

public:
    HostState(DMStateMachine& dsm, ModemProxy *modemProxy, HostAgentManager *hostAgentManager);
    virtual ~HostState()=default;
    std::string toString() const { return "HostState"; }
    //@override
    void onEnter();
    //@override
    void onExit();
    //@override
    void onModemDataReceive(const void *data, size_t datalen);
    void onModemStateChanged(bool/* online*/) {}

    static DMState *newInstance(DMStateMachine& dsm, ModemProxy *modemProxy, HostAgentManager *hostAgentManager);
};

struct SilentLoggingState : public DMStateBase {
protected:
    SilentLogging *mSilentLogging;
    int mModemState = -1;
public:
    SilentLoggingState(DMStateMachine& dsm, ModemProxy *modemProxy, SilentLogging *silentLogging);
    virtual ~SilentLoggingState()=default;
    std::string toString() const { return "SilentLoggingState"; }

    //@override
    void onEnter();
    //@override
    void onExit();
    //@override
    void onModemDataReceive(const void *data, size_t datalen);
    void onModemStateChanged(bool online);

    static DMState *newInstance(DMStateMachine& dsm, ModemProxy *modemProxy, SilentLogging *silentLogging);
};

/**
 * FactoryLoggingState
 */
struct FactoryLoggingState : public DMStateBase {
    SilentLoggingOnFactory *mFactoryLogging;
public:
    FactoryLoggingState(DMStateMachine& dsm, ModemProxy *modemProxy, SilentLoggingOnFactory *factoryLogging);
    virtual ~FactoryLoggingState()=default;
    std::string toString() const { return "FactoryLoggingState"; }

    //@override
    void onEnter();
    //@override
    void onExit();
    //@override
    void onModemDataReceive(const void *data, size_t datalen);
    void onModemStateChanged(bool online);
    static DMState *newInstance(DMStateMachine& dsm, ModemProxy *modemProxy, SilentLoggingOnFactory *factoryLogging);
};

/**
 * OnBoardAppState
 */
struct OnBoardAppState : public DMStateBase {
    OnBoardLogging *mOnBoardLogging;
    OnBoardAppState(DMStateMachine& dsm, ModemProxy *modemProxy, OnBoardLogging *onBoardLogging);
    virtual ~OnBoardAppState()=default;
    std::string toString() const { return "OnBoardAppState"; }

    //@override
    void onEnter();
    //@override
    void onExit();
    //@override
    void onModemDataReceive(const void *data, size_t datalen);
    void onModemStateChanged(bool/* online*/) {}
    static DMState *newInstance(DMStateMachine& dsm, ModemProxy *modemProxy, OnBoardLogging *onBoardLogging);
};

#endif /* __DM_STATE_IMPL_H__ */
