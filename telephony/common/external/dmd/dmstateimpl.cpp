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
 * dmstateimpl.cpp
 *
 *  Created on: 2021. 9. 15.
 */
#include "base/log.h"
#include "base/systemproperty.h"
#include "dmstateimpl.h"
#include "onboardlogging.h"
#include "silentlogging.h"
#include "HostAgentManager.h"
#include "ModemProxy.h"

/**
 * DMStateBase
 */
DMStateBase::DMStateBase(DMStateMachine& dsm, ModemProxy *modemProxy) : mDsm(dsm),
        mModemProxy(modemProxy) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * HostState
 */
HostState::HostState(DMStateMachine& dsm, ModemProxy *modemProxy,
        HostAgentManager *hostAgentManager) : DMStateBase(dsm, modemProxy),
                mHostAgentManager(hostAgentManager) {
}

void HostState::onEnter() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mModemProxy) {
        mModemProxy->StopForcibly();
    }

    if (mHostAgentManager) {
        mHostAgentManager->SetModemProxy(mModemProxy);
    }
}

void HostState::onExit() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mHostAgentManager) {
        mHostAgentManager->SetModemProxy(nullptr);
        mHostAgentManager->SetActiveAgent(nullptr);
    }
}

void HostState::onModemDataReceive(const void *data, size_t datalen) {
    if (mHostAgentManager) {
        HostAgent *active = mHostAgentManager->GetActiveAgent();
        if (active) {
            active->Write(data, datalen);
        }
    }
}

DMState *HostState::newInstance(DMStateMachine& dsm, ModemProxy *modemProxy,
        HostAgentManager *hostAgentManager) {
    return new HostState(dsm, modemProxy, hostAgentManager);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * SilentLoggingState
 */
SilentLoggingState::SilentLoggingState(DMStateMachine& dsm, ModemProxy *modemProxy,
        SilentLogging *silentLogging) : DMStateBase(dsm, modemProxy),
                mSilentLogging(silentLogging) {
}

void SilentLoggingState::onEnter() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mSilentLogging) {
        mSilentLogging->setModemProxy(mModemProxy);
        mSilentLogging->onPrepare();
    }
}

void SilentLoggingState::onExit() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mSilentLogging) {
        mSilentLogging->onDestroy();
        mSilentLogging->setModemProxy(nullptr);
    }
}

void SilentLoggingState::onModemDataReceive(const void *data, size_t datalen) {
    if (mSilentLogging) {
        mSilentLogging->onModemDataReceive(data, datalen);
    }
}

void SilentLoggingState::onModemStateChanged(bool online) {
    if (mEntered && mSilentLogging) {
        if (online) {
            LOGW("[%s] Resume logging due to modem online.", toString().c_str());
            mSilentLogging->onStart();
        } else {
            LOGW("[%s] Stop logging due to modem offline.", toString().c_str());
            mSilentLogging->onStop();
        }
    }

    if (!mEntered && SilentLogging::isAutoStartRequired() && SilentLogging::isAutoStartRequiredByConfig()) {
        LOGW("[%s] State transition to SilentLoggingState.", toString().c_str());
        mDsm.setState(this);
    }
}

DMState *SilentLoggingState::newInstance(DMStateMachine& dsm, ModemProxy *modemProxy,
        SilentLogging *silentLogging) {
    return new SilentLoggingState(dsm, modemProxy, silentLogging);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * FactoryLoggingState
 */
FactoryLoggingState::FactoryLoggingState(DMStateMachine& dsm, ModemProxy *modemProxy,
        SilentLoggingOnFactory *factoryLogging) : DMStateBase(dsm, modemProxy),
                mFactoryLogging(factoryLogging) {
}

void FactoryLoggingState::onEnter() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mFactoryLogging) {
        mFactoryLogging->setModemProxy(mModemProxy);
        mFactoryLogging->onPrepare();
    }
}

void FactoryLoggingState::onExit() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mFactoryLogging) {
        mFactoryLogging->onDestroy();
        mFactoryLogging->setModemProxy(nullptr);
    }
}

void FactoryLoggingState::onModemDataReceive(const void *data, size_t datalen) {
    if (mFactoryLogging) {
        mFactoryLogging->onModemDataReceive(data, datalen);
    }
}

void FactoryLoggingState::onModemStateChanged(bool online) {
    if (mEntered && mFactoryLogging) {
        if (online) {
            LOGW("[%s] Resume logging due to modem online.", toString().c_str());
            mFactoryLogging->onStart();
        } else {
            LOGW("[%s] Stop logging due to modem offline.", toString().c_str());
            mFactoryLogging->onStop();
        }
    }
}

DMState *FactoryLoggingState::newInstance(DMStateMachine& dsm, ModemProxy *modemProxy,
        SilentLoggingOnFactory *factoryLogging) {
    return new FactoryLoggingState(dsm, modemProxy, factoryLogging);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * OnBoardAppState
 */
OnBoardAppState::OnBoardAppState(DMStateMachine& dsm, ModemProxy *modemProxy,
        OnBoardLogging *onBoardLogging) :
        DMStateBase(dsm, modemProxy), mOnBoardLogging(onBoardLogging) {
}

void OnBoardAppState::onEnter() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mOnBoardLogging) {
        mOnBoardLogging->setModemProxy(mModemProxy);
    }
}

void OnBoardAppState::onExit() {
    LOGD("[%s] %s", toString().c_str(), __FUNCTION__);
    if (mModemProxy) {
        mModemProxy->StopForcibly();
    }

    if (mOnBoardLogging) {
        mOnBoardLogging->setModemProxy(nullptr);
    }
}

void OnBoardAppState::onModemDataReceive(const void *data, size_t datalen) {
    if (mOnBoardLogging) {
        mOnBoardLogging->onModemDataReceive(data, datalen);
    }
}

DMState *OnBoardAppState::newInstance(DMStateMachine& dsm, ModemProxy *modemProxy,
        OnBoardLogging *onBoardLogging) {
    return new OnBoardAppState(dsm, modemProxy, onBoardLogging);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
