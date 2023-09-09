 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __NETWORK_SERVICE_BASE_H__
#define __NETWORK_SERVICE_BASE_H__

#include "service.h"
#include "ts25table.h"
#include "eonsResolver.h"
#include "telephonystatustracker.h"

#define TIMEOUT_NET_DEFAULT                     30000
#define TIMEOUT_NET_QUERY_AVAILABLE_NETWORK     300000 // changed 300sec. from 2min 20150721 <- CP timeout was 20xsec
#define TIMEOUT_NET_SET_NETWORK_MODE            120000 // 2 min.
#define MAX_UUID_LENGTH                         64

enum {
    RADIO_NOT_READY,
    RADIO_READY,
    RADIO_WAITING,
};

class EmergencyNumberManager;
class EccInfo;
class Message;
class NetworkServiceProxy;
class RilContext;

class NetworkServiceBase : public Service, public MonitoringCallback
{
    friend NetworkServiceProxy;
protected:
    int                m_nRadioReady;
    RIL_RadioState    m_radioState;
    RIL_RadioState    m_desiredRadioState;
    bool              mDelayedRadioPower;
    RIL_RadioTechnology    m_nVoiceRat;
    RIL_RadioTechnology    m_nDataRat;
    int m_nRegState;
    int m_nDataRegState;

    int m_cardState;
    int m_appState;

    bool m_bShutdown;

    bool mIsNrTestMode;

    EmergencyNumberManager *mEmergencyNumberManager;
    bool mIsDisabledESim;

  public:
    NetworkServiceBase(RilContext* pRilContext);
    virtual ~NetworkServiceBase();

public:
    int GetRadioReady() { return m_nRadioReady; }
    void SetRadioReady(int state);
    RIL_RadioState GetRadioState() { return m_radioState; }
    void UpdateRadioState(RIL_RadioState radioState, bool notifyResult = false);
    void TrySetRadioPower(int desiredRadioState);
    void setVoiceRat(RIL_RadioTechnology);
    EccInfo *GetRadioEccInfos();
    void UpdateDbEccInfo(bool forceToUpdate = true, bool callByOpposite = false);
    void UpdateEmergencyNumberList();
    bool IsEmergencyCallRouting(const char *number, bool *normalRoutingWhenInService);
    int GetEmergencyNumberCategory(const char *number);
    void SetEmcLabTestMode(bool isEmcLabTestMode);
    void ResetDbEccId(bool forceToErase = false);
    void SetTempEmergencyNumber(int operation, int number_len = 0, char *number = NULL, int category = 0);
    inline string GetSimOperatorNumeric() { return Service::GetSimOperatorNumeric(); }
    inline Service* GetCurrentService(int nServiceId) { return Service::GetCurrentService(nServiceId); }
    inline Service* GetOppositeService(int nServiceId) { return Service::GetOppositeService(nServiceId); }

private:
    int mCurrentSim;

protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnStart();

    virtual void OnReset();
    virtual void OnModemOnline();
    virtual void OnRadioStateChanged(int radioState);
    virtual void OnRadioNotAvailable();
    virtual void OnRadioOffOrNotAvailable();
    virtual void OnRadioAvailable();
    virtual void OnSimStatusChanged(int cardState, int appState);
    virtual void OnServiceStateChanged(const ServiceState& state);
    virtual int OnRadioReady(Message *pMsg);
    virtual bool IsPossibleToPassInRadioOffState(int request_id);
    virtual bool IsPossibleToPassInRadioUnavailableState(int request_id);
    virtual void OnImsiUpdated(const char *imsi);

protected:
    void OnRadioReady();
    void WriteVolteEmcServiceStatus(bool volteAvilable, int emcService, int rat, int regState, bool notify = false);
    void QueryCurrentPsDomainState();
    void QueryEmergencyCallAvailableRadioTech();
    bool IsDisabledESim();

public:
    void dump(std::stringstream& ss);
};

#endif /*__NETWORK_SERVICE_BASE_H__*/
