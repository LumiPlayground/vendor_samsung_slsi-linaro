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
 * servicecallback.h
 *
 *  Created on: 2021. 9. 24.
 */

#ifndef __SERVICE_CALLBACK_H__
#define __SERVICE_CALLBACK_H__

#include "registrant.h"

class ServiceState;
class SimCardStatus;

namespace ServiceCallback {

enum class RadioState : int32_t {
    Off = 0,
    Unavailable = 1,
    On = 10,
};


/**
 * RadioStateListener
 */
struct RadioStateListener {
    virtual ~RadioStateListener()=default;
    virtual void OnRadioStateChanged(int/* state*/) {}
    virtual void OnRadioNotAvailable() {}
    virtual void OnRadioOffOrNotAvailable() {}
    virtual void OnRadioAvailable() {}
    virtual void OnRadioOn() {}
};

enum class ModemState : int32_t {
    Offline = 0,
    Online = 4,
};

/**
 * ModemStateListener
 */
struct ModemStateListener {
    virtual ~ModemStateListener()=default;
    virtual void OnModemStateChanged(int/* state*/) {}
    virtual void OnModemOnline() {}
    virtual void OnModemOffline() {}
};

/**
 * NetworkStateListener
 */
struct NetworkStateListener {
    virtual ~NetworkStateListener()=default;
    virtual void OnVoiceRegistrationStateChanged(int/* regState*/) {}
    virtual void OnDataRegistrationStateChanged(int/* regState*/) {}
    virtual void OnServiceStateChanged(const ServiceState&/* serviceState*/) {}
};

/**
 * ImsRegistrationStateListener
 */
struct ImsRegistrationStateListener {
    virtual ~ImsRegistrationStateListener()=default;
    virtual void OnImsRegistrationStateChanged(int /*regState*/) {}
};

/**
 * SimStatusListener
 */
struct SimStatusListener {
    virtual ~SimStatusListener()=default;
    virtual void OnSimStatusChanged(int/* cardState*/, int/* appState*/) {}
    virtual void OnSimStatusChanged(const SimCardStatus&/* cardStatus*/) {}
};

}  // namespace ServiceCallback

#endif /* __SERVICE_CALLBACK_H__ */
