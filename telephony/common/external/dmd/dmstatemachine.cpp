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
 * dmstatemachine.cpp
 *
 *  Created on: 2021. 9. 14.
 */
#include "base/log.h"
#include "dmstatemachine.h"
#include "DMConstants.h"

struct UnavailableState : public DMState {
    DMStateMachine& mDsm;
    UnavailableState(DMStateMachine& dsm) : mDsm(dsm) {}
    void enter() { LOGD("UnavailableState::enter"); }
    void exit() { LOGD("UnavailableState::exit"); }
    std::string toString() const { return "UnavailableState"; }
    void onModemDataReceive(const void */*data*/, size_t/* datalen*/) {}
    void onModemStateChanged(bool/* online*/) {}
};

DMStateMachine::DMStateMachine() : mCurrentState(nullptr) {
}

DMStateMachine::~DMStateMachine() {
    for (auto it = mStates.begin(); it != mStates.end(); it++) {
        if (it->second) {
            delete it->second;
        }
    }
}

void DMStateMachine::init(DMStateFactory *factory) {
    if (factory) {
        addState(DM_STATE_UNAVAILABE_STATE, new UnavailableState(*this));
        addState(DM_STATE_HOST_DM_STATE, factory->createHostState(*this));
        addState(DM_STATE_SIELNT_LOGGING_STATE, factory->createSilentLoggingState(*this));
        addState(DM_STATE_ON_BOARD_APP_STATE, factory->createOnBoardAppState(*this));
        addState(DM_STATE_FACTORY_DM_STATE, factory->createFactoryState(*this));
    }
    // default
    setState(DM_STATE_HOST_DM_STATE);
}

DMState *DMStateMachine::getCurrentState() {
    return mCurrentState;
}

void DMStateMachine::addState(int id, DMState *state) {
    if (state) {
        mStates.insert(std::pair<int, DMState *>(id, state));
    }
}

void DMStateMachine::setState(DMState *state) {
    // not to allow a transition to the same state
    if (mCurrentState != state) {
        if (mCurrentState) {
            mCurrentState->exit();
        }
        addHistory(mCurrentState);
        mCurrentState = state;
        if (mCurrentState) {
            mCurrentState->enter();
        }
    }
}

DMState *DMStateMachine::getState(int state) {
    DMState *ret = nullptr;
    auto it = mStates.find(state);
    if (it != mStates.end()) {
        ret = it->second;
    }
    if (ret == nullptr) {
        LOGW("[DMStateMachine] No available DMState for %d.", state);
    }
    return ret;
}

DMState *DMStateMachine::getLastState() {
    DMState *state = nullptr;
    if (mHistory.size() > 0) {
        state = mHistory.back();
    }
    LOGD("[DMStateMachine] getLastState %s", state ? state->toString().c_str() : "null");
    return state;
}

void DMStateMachine::addHistory(DMState *state) {
    mHistory.push_back(state);
    LOGD("[DMStateMachine] addHistory %s", state ? state->toString().c_str() : "null");
    if (mHistory.size() > HISTORY_SIZE) {
        mHistory.pop_front();
    }
}

//@override
void DMStateMachine::onReceive(const void *data, size_t datalen) {
    if (mCurrentState) {
        mCurrentState->onModemDataReceive(data, datalen);
    }
}

void DMStateMachine::onModemStateChanged(int state) {
    int oldState = mModemState;
    mModemState = (state == ModemStateMonitor::State::STATE_ONLINE);

    if (oldState != mModemState) {
        for (auto it = mStates.begin(); it != mStates.end(); it++) {
            if (it->second) {
                it->second->onModemStateChanged(mModemState);
            }
        }
    }
}
