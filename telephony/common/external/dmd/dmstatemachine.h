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
 * dmstatemachine.h
 *
 *  Created on: 2021. 9. 14.
 */

#ifndef __DM_STATE_MACHINE_H__
#define __DM_STATE_MACHINE_H__

#include "ModemProxy.h"
#include <list>
#include <map>
#include <string>

class DMStateMachine;

enum {
    DM_STATE_UNKNOWN = 0,
    DM_STATE_HOST_DM_STATE = 1,
    DM_STATE_SIELNT_LOGGING_STATE = 2,
    DM_STATE_ON_BOARD_APP_STATE = 3,
    DM_STATE_FACTORY_DM_STATE = 4,
    DM_STATE_UNAVAILABE_STATE = 5,
};

struct DMState {
    virtual ~DMState()=default;
    virtual std::string toString() const=0;
    virtual void enter()=0;
    virtual void exit()=0;
    virtual void onModemDataReceive(const void *data, size_t datalen)=0;
    virtual void onModemStateChanged(bool online)=0;
};

struct DMStateFactory {
    virtual ~DMStateFactory()=default;
    virtual DMState *createHostState(DMStateMachine& dsm)=0;
    virtual DMState *createSilentLoggingState(DMStateMachine& dsm)=0;
    virtual DMState *createOnBoardAppState(DMStateMachine& dsm)=0;
    virtual DMState *createFactoryState(DMStateMachine& dsm)=0;
};

class DMStateMachine : public ModemDataReceiver, public ModemStateLinstener {
    DMState *mCurrentState;
    std::map<int, DMState *> mStates;
    std::list<DMState *> mHistory;
    static const size_t HISTORY_SIZE = 10;
    int mModemState = -1;

public:
    DMStateMachine();
    ~DMStateMachine();

    void init(DMStateFactory *factory);
    DMState *getCurrentState();
    void addState(int id, DMState *state);
    void setState(DMState *state);
    void setState(int state) { setState(getState(state)); }
    DMState *getState(int state);
    DMState *getLastState();
    void addHistory(DMState *state);

    //@override
    void onReceive(const void *data, size_t datalen);

    void onModemStateChanged(int state);
};

#endif /* __DM_STATE_MACHINE_H__ */
