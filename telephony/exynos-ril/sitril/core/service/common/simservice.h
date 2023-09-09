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
 * simservice.cpp
 *
 *  Created on: 2014. 7. 1.
 *      Author: MOX
 */

#ifndef __SIM_SERVICE_H__
#define __SIM_SERVICE_H__

#include "protocolsimadapter.h"
#include "service.h"
#include "telephonystatustracker.h"
#include <slsi/radio_v1_5.h>
#include <slsi/radio_v1_6.h>
#include <slsi/radio_v2_0.h>

#define    TIMEOUT_SIM_DEFAULT        30000
#define    TIMEOUT_SIM_ACT_DEACT      30000

#define BUFFER_SIZE (128)
#define INVALID_PIN_LEN (-1)
#define INVALID_PIN_STR ""

//#define _SIM_DEPERSONALIZATION_TEST_

using namespace android::hardware::radio::impl::sim;

class Message;
class RilContext;
class ModemData;
class StkService;
class SimIoService;
class SimServiceProxy;
class SimLockHandler;
class UiccPhonebookHandler;
class SimAutoVerifyPinHandler;

class SimService : public Service, public MonitoringCallback
{
    INTERFACE_AS_SERVICE(SimService, RIL_SERVICE_SIM)
    friend StkService;
    friend SimIoService;
    friend SimServiceProxy;
    friend SimLockHandler;
    friend UiccPhonebookHandler;
    friend SimAutoVerifyPinHandler;

protected:
    // Member Variables

    // This Card State is used in BuildSimStatus() only for HotSwap
    typedef enum {
        SIM_CARDSTATE_UNKNOWN,
        SIM_CARDSTATE_ABSENT,
        SIM_CARDSTATE_PRESENT,
        SIM_CARDSTATE_ERROR,
    } SIM_CARD_STATE;

public:
    typedef enum {
        UICC_APP_STATE_DISABLED,
        UICC_APP_STATE_ENABLED,
    } UICC_APP_STATE;

    typedef enum {
        SIM_PIN,
        SIM_PUK
    } SIM_PIN_PUK;

    typedef enum {
        PIN1_PUK1 = 0,
        PIN2_PUK2
    } SIM_PIN_PUK_INDEX;

    typedef enum {
        SIM_CARD_POWERSTATE_POWER_DOWN,
        SIM_CARD_POWERSTATE_POWER_UP,
        SIM_CARD_POWERSTATE_POWER_UP_PASS_THROUGH,
        SIM_CARD_POWERSTATE_POWER_RESET, // Added for secure element operation in Radio v1.2
        SIM_CARD_POWERSTATE_POWER_DOWN_PRESENT, // AP send power down state to cp and expect to
        // receive sim state as present in Radio v1.6
    } SIM_CARD_POWER_STATE;

public:
    SimService(RilContext* pRilContext);
    SimService(const SimService &) = delete;
    SimService& operator=(SimService const&) = delete;
    virtual ~SimService();

protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnStart();

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleRequestTimeout(Message* pMsg);

    virtual void OnReset();
    virtual void OnSimStatusChanged(int cardState, int appState);
    virtual void OnRadioStateChanged(int radioState);
    virtual void OnRadioNotAvailable();
    virtual void OnModemStateChanged(int state);

    virtual bool IsPossibleToPassInRadioOffState(int request_id);

protected:
    virtual int DoGetSimStatus(Message *pMsg);
    virtual int CheckAndAutoVerifyPin(UINT uErrCode, void *pResponse, int nFinalLength);
    virtual int OnGetSimStatusDone(Message *pMsg);
    virtual int OnGetSimStatusTimeout(Message *pMsg);

    char *BuildSimStatus(ProtocolSimStatusAdapter &adapter);
    void InitRilCardStatus();
    void BuildRilCardStatusApplications(ProtocolSimStatusAdapter &adapter);
    void FillRilCardStatusFromAdapter(ProtocolSimStatusAdapter &adapter, bool isBuildNeeded);
    bool IsEarlySimDetection(ProtocolSimStatusAdapter &adapter);
    virtual int OnSimHotSwap(BOOL bRemoval);

    int DoAutoDisableUicc();
    int OnAutoDisableUiccDone(Message *pMsg);
    int CheckDisabledIccid();

    SimLockHandler *GetSimLockHandler() { return m_pSimLockHandler; }
    SimAutoVerifyPinHandler *GetSimAutoVerifyPinHandler() { return m_pSimAutoVerifyPinHandler; }

private:
    SimLockHandler *m_pSimLockHandler;
    UiccPhonebookHandler *m_pUiccPbHandler;
    SimAutoVerifyPinHandler *m_pSimAutoVerifyPinHandler;

    // Internal SIM card state; External module should not be access
    int m_nSimCardState;

protected:
    //RIL_AppStatus mRilAppStatus[RIL_CARD_MAX_APPS];
    V2_0::RIL_CardStatus mRilCardStatus;
    string previousIccId, orderedIccId;
    char m_szAtr[MAX_ATR_LEN*2 + 1];
    char m_szIccid[MAX_ICCID_LEN*2 + 1];
    char m_szEid[MAX_EID_LEN*2 + 1];

    char m_aszAID[RIL_CARD_MAX_APPS][36];        // AID string for mRilCardStatus's pointer

    INT32 m_nRadioState;

    //Radio version 1.5, UiccApplication enablement status
    int m_nCurrentUiccEnableState;   // 1 is for enabled, 0 is for disbled
    // variable used to ensure that duplicate indication is not sent to f/w
    bool m_bUiccApplicationsEnablementChanged;
    int m_nReqUiccEnableState;
    char m_szLastDisabledIccid[BUFFER_SIZE+1];

    int m_nImsAppType;

    typedef enum {
        BOOT_STATE_NORMAL,
        BOOT_STATE_KERNEL_PANIC,
        BOOT_STATE_UNKNOWN,
    } BOOT_STATE;

    int m_nBootState;

    int m_nPrevAppState;
    bool m_bSendVoiceRat;
    string mImsi;

public:
    virtual V2_0::RIL_CardStatus GetRilCardStatus();
    int GetGsmAppState();
    void dump(std::stringstream& ss);

};

#endif /*__SIM_SERVICE_H__*/
