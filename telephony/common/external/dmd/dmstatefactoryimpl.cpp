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
 * dmstatefactoryimpl.cpp
 *
 *  Created on: 2021. 9. 14.
 */
#include "dmstatefactoryimpl.h"
#include "dmstateimpl.h"

DMStateFactoryImpl::DMStateFactoryImpl() : mModemProxy{nullptr}, mHostAgentManager{nullptr},
        mSilentLogging{nullptr}, mOnBoardLogging{nullptr}, mFactoryLogging{nullptr}  {
}

DMState *DMStateFactoryImpl::createHostState(DMStateMachine& dsm) {
    return HostState::newInstance(dsm, mModemProxy, mHostAgentManager);
}

DMState *DMStateFactoryImpl::createSilentLoggingState(DMStateMachine& dsm) {
    return SilentLoggingState::newInstance(dsm, mModemProxy, mSilentLogging);
}

DMState *DMStateFactoryImpl::createOnBoardAppState(DMStateMachine& dsm) {
    return OnBoardAppState::newInstance(dsm, mModemProxy, mOnBoardLogging);
}

DMState *DMStateFactoryImpl::createFactoryState(DMStateMachine& dsm) {
    return FactoryLoggingState::newInstance(dsm, mModemProxy, mFactoryLogging);
}

DMStateFactory *DMStateFactoryImpl::getInstance(ModemProxy *modemProxy,
        HostAgentManager *hostAgentManager, SilentLogging *silentLogging,
        SilentLoggingOnFactory *factoryLogging) {
    DMStateFactoryImpl *factory = new DMStateFactoryImpl();
    factory->mModemProxy = modemProxy;
    factory->mHostAgentManager = hostAgentManager;
    factory->mSilentLogging = silentLogging;
    factory->mFactoryLogging = factoryLogging;
    return factory;
}

DMStateFactory *DMStateFactoryImpl::getInstance(ModemProxy *modemProxy,
        HostAgentManager *hostAgentManager, SilentLogging *silentLogging,
        OnBoardLogging *onBoardLogging, SilentLoggingOnFactory *factoryLogging) {
    DMStateFactoryImpl *factory = new DMStateFactoryImpl();
    factory->mModemProxy = modemProxy;
    factory->mHostAgentManager = hostAgentManager;
    factory->mSilentLogging = silentLogging;
    factory->mOnBoardLogging = onBoardLogging;
    factory->mFactoryLogging = factoryLogging;
    return factory;
}
