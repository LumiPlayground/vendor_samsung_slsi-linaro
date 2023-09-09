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
 * servicecallbacknotifier.h
 *
 *  Created on: 2021. 9. 27.
 */
#ifndef __SERVICE_CALLBACK_NOTIFIER_H__
#define __SERVICE_CALLBACK_NOTIFIER_H__

#include "servicecallback.h"

namespace ServiceCallback {

/**
 * RadioStateNotifier
 */
struct RadioStateNotifier {
    RegistrantList<ServiceCallback::RadioStateListener *> mListener;
    void addListener(ServiceCallback::RadioStateListener *lsn) { mListener.add(lsn); }
    void removeListener(ServiceCallback::RadioStateListener *lsn) { mListener.remove(lsn); }
    void removeAll() { mListener.clear(); }

    void notifyRadioStateChanged(int oldState, int newState, bool forceNotify = false);
    void notifyRadioStateChanged(int state);
    void notifyRadioNotAvailable();
    void notifyRadioOffOrNotAvailable();
    void notifyRadioAvailable();
    void notifyRadioOn();
};

/**
 * ModemStateNotifier
 */
struct ModemStateNotifier {
    RegistrantList<ServiceCallback::ModemStateListener *> mListener;
    void addListener(ServiceCallback::ModemStateListener *lsn) { mListener.add(lsn); }
    void removeListener(ServiceCallback::ModemStateListener *lsn) { mListener.remove(lsn); }
    void removeAll() { mListener.clear(); }

    void notifyModemStateChanged(int oldState, int newState);
    void notifyModemStateChanged(int state);
    void notifyModemOffline();
    void notifyModemOnline();
};


/**
 * NetworkStateNotifier
 */
struct NetworkStateNotifier {
    RegistrantList<ServiceCallback::NetworkStateListener *> mListener;
    void addListener(ServiceCallback::NetworkStateListener *lsn) { mListener.add(lsn); }
    void removeListener(ServiceCallback::NetworkStateListener *lsn) { mListener.remove(lsn); }
    void removeAll() { mListener.clear(); }

    void notifyVoiceRegistrationStateChanged(int regState);
    void notifyDataRegistrationStateChanged(int regState);
    void notifyServiceStateChanged(const ServiceState& serviceState);
};

/**
 * SimStatusNotifier
 */
struct SimStatusNotifier {
    RegistrantList<ServiceCallback::SimStatusListener *> mListener;
    void addListener(ServiceCallback::SimStatusListener *lsn) { mListener.add(lsn); }
    void removeListener(ServiceCallback::SimStatusListener *lsn) { mListener.remove(lsn); }
    void removeAll() { mListener.clear(); }

    void notifySimStatusChanged(int cardState, int appState);
    void notifySimStatusChanged(const SimCardStatus& cardStatus);
};

}  // namespace ServiceCallback
#endif /* __SERVICE_CALLBACK_NOTIFIER_H__ */
