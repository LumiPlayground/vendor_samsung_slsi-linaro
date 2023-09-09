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
 * servicecallback.cpp
 *
 *  Created on: 2021. 9. 24.
 *      Author: sungwoo48.choi
 */
#include "modemstatemonitor.h"
#include "servicecallback.h"
#include "servicecallbacknotifier.h"
#include <telephony/ril.h>

namespace ServiceCallback {

/**
 * RadioStateNotifier
 */
void RadioStateNotifier::notifyRadioStateChanged(int oldState, int newState,
        bool forceNotify/* = false*/) {
    if (oldState != newState || forceNotify) {
        notifyRadioStateChanged(newState);
    }

    if (RadioState(newState) != RadioState::Unavailable &&
            RadioState(oldState) == RadioState::Unavailable) {
        notifyRadioAvailable();
    }

    if (RadioState(newState) == RadioState::Unavailable &&
            RadioState(oldState) != RadioState::Unavailable) {
        notifyRadioNotAvailable();
    }

    if (RadioState(newState) == RadioState::On &&
            RadioState(oldState) != RadioState::On) {
        notifyRadioOn();
    }

    if ((RadioState(newState) == RadioState::Off ||
        RadioState(newState) == RadioState::Unavailable) &&
            RadioState(oldState) == RadioState::On) {
        notifyRadioOffOrNotAvailable();
    }
}

void RadioStateNotifier::notifyRadioStateChanged(int state) {
    using T = ServiceCallback::RadioStateListener *;
    struct LocalExecutor : public Executor<T> {
        int mState;
        LocalExecutor(int state) : mState(state) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnRadioStateChanged(mState);
            }
        }
    } executor(state);
    mListener.notifyAll(&executor);
}

void RadioStateNotifier::notifyRadioNotAvailable() {
    using T = ServiceCallback::RadioStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnRadioNotAvailable();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

void RadioStateNotifier::notifyRadioOffOrNotAvailable() {
    using T = ServiceCallback::RadioStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnRadioOffOrNotAvailable();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

void RadioStateNotifier::notifyRadioAvailable() {
    using T = ServiceCallback::RadioStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnRadioAvailable();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

void RadioStateNotifier::notifyRadioOn() {
    using T = ServiceCallback::RadioStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnRadioOn();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

/**
 * ModemStateNotifier
 */
void ModemStateNotifier::notifyModemStateChanged(int oldState, int newState) {
    if (oldState != newState) {
        notifyModemStateChanged(newState);

        if (ModemState(newState) == ModemState::Online &&
                ModemState(oldState) != ModemState::Online) {
            notifyModemOnline();
        }

        if (ModemState(newState) != ModemState::Online &&
                ModemState(oldState) == ModemState::Online) {
            notifyModemOffline();
        }
    }
}

void ModemStateNotifier::notifyModemStateChanged(int state) {
    using T = ServiceCallback::ModemStateListener *;
    struct LocalExecutor : public Executor<T> {
        int mState;
        LocalExecutor(int state) : mState(state) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnModemStateChanged(mState);
            }
        }
    } executor(state);
    mListener.notifyAll(&executor);
}

void ModemStateNotifier::notifyModemOffline() {
    using T = ServiceCallback::ModemStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnModemOffline();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

void ModemStateNotifier::notifyModemOnline() {
    using T = ServiceCallback::ModemStateListener *;
    struct LocalExecutor : public Executor<T> {
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnModemOnline();
            }
        }
    } executor;
    mListener.notifyAll(&executor);
}

/**
 * NetworkStateNotifier
 */
void NetworkStateNotifier::notifyVoiceRegistrationStateChanged(int regState) {
    using T = ServiceCallback::NetworkStateListener *;
    struct LocalExecutor : public Executor<T> {
        int mState;
        LocalExecutor(int state) : mState(state) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnVoiceRegistrationStateChanged(mState);
            }
        }
    } executor(regState);
    mListener.notifyAll(&executor);
}

void NetworkStateNotifier::notifyDataRegistrationStateChanged(int regState) {
    using T = ServiceCallback::NetworkStateListener *;
    struct LocalExecutor : public Executor<T> {
        int mState;
        LocalExecutor(int state) : mState(state) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnDataRegistrationStateChanged(mState);
            }
        }
    } executor(regState);
    mListener.notifyAll(&executor);
}

void NetworkStateNotifier::notifyServiceStateChanged(const ServiceState& serviceState) {
    using T = ServiceCallback::NetworkStateListener *;
    struct LocalExecutor : public Executor<T> {
        const ServiceState& mServiceState;
        LocalExecutor(const ServiceState& serviceState) : mServiceState(serviceState) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnServiceStateChanged(mServiceState);
            }
        }
    } executor(serviceState);
    mListener.notifyAll(&executor);
}

/**
 * SimStatusNotifier
 */
void SimStatusNotifier::notifySimStatusChanged(int cardState, int appState) {
    using T = ServiceCallback::SimStatusListener *;
    struct LocalExecutor : public Executor<T> {
        int mCardState;
        int mAppState;
        LocalExecutor(int cardState, int appState) : mCardState(cardState), mAppState(appState) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnSimStatusChanged(mCardState, mAppState);
            }
        }
    } executor = { cardState, appState };
    mListener.notifyAll(&executor);
}

void SimStatusNotifier::notifySimStatusChanged(const SimCardStatus& cardStatus) {
    using T = ServiceCallback::SimStatusListener *;
    struct LocalExecutor : public Executor<T> {
        const SimCardStatus& mCardStatus;
        LocalExecutor(const SimCardStatus& cardStatus) : mCardStatus(cardStatus) {}
        void run(T&& lsn) {
            if (lsn) {
                lsn->OnSimStatusChanged(mCardStatus);
            }
        }
    } executor = { cardStatus };
    mListener.notifyAll(&executor);
}

}  // namespace ServiceCallback

