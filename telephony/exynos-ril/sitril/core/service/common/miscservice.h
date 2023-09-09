 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MISCSERVICE_H__
#define __MISCSERVICE_H__

#include "service.h"
#include "telephonystatustracker.h"

#define TIMEOUT_MISC_DEFAULT        5000

class Message;
class MiscServiceProxy;

class MiscService : public Service, public MonitoringCallback
{
    INTERFACE_AS_SERVICE(MiscService, RIL_SERVICE_MISC)
    friend MiscServiceProxy;
public:
    MiscService(RilContext* pRilContext);
    virtual ~MiscService();

protected:
    static const int MISC_TIMEOUT = 5000;
    static const int IMS_DEFAULT_TIMEOUT = 5000;

    int m_nSubscription;
    bool mDontReportOemSignalStrength;
    RIL_SignalStrength_V1_4 *mCurrentSignalStrength;
    uint64_t mLastReceivedTimestamp;

    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnStart();

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleInternalMessage(Message* pMsg);

    virtual void OnSimStatusChanged(int cardState, int appState);
    virtual void OnModemStateChanged(int state);
    virtual void OnRadioOffOrNotAvailable();
    virtual void OnRadioAvailable();
    virtual void OnReset();
    virtual bool IsPossibleToPassInRadioOffState(int request_id);
    virtual bool IsPossibleToPassInRadioUnavailableState(int request_id);

protected:
    virtual int DoSignalStrength(Message *pMsg);
    virtual int OnSignalStrengthDone(Message *pMsg);
    virtual int DoOEMSysDump(Message *pMsg);
    virtual int DoRecievedTestJsonData(Message *pMsg);

    //handle unsolicited response
    virtual int OnUnsolSignalStrength(Message *pMsg);

    int ConvertRssiToAsu(INT32 rssi);
    int DoForceCpCrash(LogDumpCause crash_reason = LOG_DUMP_CAUSE_CP_CRASH_APP, int _info = 0);

    void RequestDeviceReset(int reset_type);
    void SetDebugTraceOffOnBoot();
    void SetModemsConfig();
    void SendDeviceInfo();
    void SendSGCValue();
    void SendSvnInfo();
    void SetFeatureInfo();
    void SetSlotMapping();

    void UpdateCurrentSignalStrength(RIL_SignalStrength_V1_4 *signalStrength);
    void PrintCurrentSignalStrength();
    bool IsQueryingSignalStrengthNeed();
    void OnNotifyModemCrashState(bool alert);
    void InitPropertyForGidValue(int cardState);

private:
    int mCardState;
    int mAppState;
    int mModemState;
    bool mNeedModemStateRecovery;

    typedef enum {
        SIM_CARDSTATE_UNKNOWN,
        SIM_CARDSTATE_ABSENT,
        SIM_CARDSTATE_PRESENT,
        SIM_CARDSTATE_ERROR,
    } SIM_CARD_STATE;

public:
    MiscService (const MiscService&) = delete;
    MiscService& operator=(const MiscService&) = delete;
    int GetUiccCardState() { return mCardState; }
    int GetCdmaSubscriptionSource() { return m_nSubscription; }
    void SetCdmaSubscriptionSource(int subscriptionSource) { m_nSubscription = subscriptionSource; }
    void dump(std::stringstream& ss);
    void SetBtlBufMaxSize();
};

#endif
