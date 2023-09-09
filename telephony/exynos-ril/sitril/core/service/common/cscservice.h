/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __CSCSERVICE_H__
#define __CSCSERVICE_H__

#include "service.h"
#include "callreqdata.h"
#include "telephonystatustracker.h"

class Message;
class CscServiceProxy;
class CallList;
class EmergencyCallHelper;

class CscService : public Service, public MonitoringCallback
{
    INTERFACE_AS_SERVICE(CscService, RIL_SERVICE_CSC)
    friend CscServiceProxy;
public:
    CscService(RilContext* pRilContext);
    virtual ~CscService();

protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);

protected:
    virtual bool IsPossibleToPassInRadioOffState(int request_id);
    virtual bool IsPossibleToPassInRadioUnavailableState(int request_id);
    virtual void OnVoiceRegistrationStateChanged(int regState);
    virtual void OnImsRegistrationStateChanged(int regState);

private:
    CallId m_CallId;
    CallList *m_currCallList;
    EmergencyCallHelper *m_emergencyCallHelper;

public:
    BOOL IsInCallState();
    bool IsRadioOnForEmergencyCall();
    void SetRadioOnForEmergencyCall(bool radioOnForEmergencyCall);
    inline string GetSimOperatorNumeric() { return Service::GetSimOperatorNumeric(); }
    void dump(std::stringstream& ss);
};
#endif
