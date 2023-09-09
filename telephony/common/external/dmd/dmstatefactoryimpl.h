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
 * dmstatefactoryimpl.h
 *
 *  Created on: 2021. 9. 14.
 */

#ifndef __DM_STATE_FACTORY_IMPL_H__
#define __DM_STATE_FACTORY_IMPL_H__

#include "dmstatemachine.h"
#include "ModemProxy.h"
#include "HostAgentManager.h"
#include "onboardlogging.h"
#include "silentlogging.h"

struct DMStateFactoryImpl : public DMStateFactory {
    ModemProxy *mModemProxy;
    HostAgentManager *mHostAgentManager;
    SilentLogging *mSilentLogging;
    OnBoardLogging *mOnBoardLogging;
    SilentLoggingOnFactory *mFactoryLogging;

    DMStateFactoryImpl();
    virtual ~DMStateFactoryImpl()=default;
    DMState *createHostState(DMStateMachine& dsm);
    DMState *createSilentLoggingState(DMStateMachine& dsm);
    DMState *createOnBoardAppState(DMStateMachine& dsm);
    DMState *createFactoryState(DMStateMachine& dsm);

    static DMStateFactory *getInstance(ModemProxy *modemProxy, HostAgentManager *hostAgentManager,
            SilentLogging *silentLogging, SilentLoggingOnFactory *factoryLogging);
    static DMStateFactory *getInstance(ModemProxy *modemProxy, HostAgentManager *hostAgentManager,
            SilentLogging *silentLogging, OnBoardLogging *onBoardLogging,
            SilentLoggingOnFactory *factoryLogging);
};


#endif /* __DM_STATE_FACTORY_IMPL_H__ */
