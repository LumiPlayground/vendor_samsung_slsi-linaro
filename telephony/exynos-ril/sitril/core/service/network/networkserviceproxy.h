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
 * networkserviceproxy.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __NETWORK_SERVICE_PROXY_H__
#define __NETWORK_SERVICE_PROXY_H__

class CscService;
class NetworkService;
class Service;
class ServiceState;

class NetworkServiceProxy {

private:
    NetworkService *mService;

public:
    NetworkServiceProxy(Service *service);
    ~NetworkServiceProxy();

public:
    bool HasService() { return mService != NULL; }
    int GetRadioReady();
    void SetRadioReady(int state);
    int GetRadioState();
    void UpdateRadioState(int radioState, bool notifyResult = false);
    int GetDesiredRadioState();
    void SetDesiredRadioState(int desiredRadioState);
    bool IsDelayedRadioPower();
    void SetDelayedRadioPower(bool delayed);
    bool IsRadioShutdown();
    void SetRadioShutdown(bool flag);
    int GetVoiceRegistrationState();
    void SetVoiceRegistrationState(int state);
    int GetDataRegistrationState();
    void SetDataRegistrationState(int state);
    int GetVoiceRadioTech();
    void SetVoiceRadioTech(int rat);
    int GetDataRadioTech();
    void SetDataRadioTech(int rat);
    int GetImsRegistrationState();
    void NotifyVoiceRegistrationFailed(int regState, int rejectCause);
    void WriteVolteEmcServiceStatus(bool volteAvilable, int emcService, int rat, int regState, bool notify = false);
    bool IsNrTestMode();
    bool IsIwlanEnabled();
    void SetIwlanEnabled(bool enabled);
    void UpdateDbEccInfo(bool forceToUpdate = true, bool callByOpposite = false);
    UINT IsOppsiteStackOccupyRF();
    UINT IsCurrentStackOccupyRF();
    CscService* FindCscService();
};

#endif /* __NETWORK_SERVICE_PROXY_H__ */
