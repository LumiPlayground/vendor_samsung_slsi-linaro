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
 * smsservice.h
 *
 *  Created on: 2014. 7. 21.
 *      Author: sungwoo48.choi
 */
#ifndef __SMS_SERVICE_H__
#define __SMS_SERVICE_H__

#include "service.h"

#define SMS_DEFAULT_TIMEOUT        10000
#define SMS_SEND_TIMEOUT        200000

class Message;
class RilContext;
class SmsServiceProxy;

class SmsService : public Service
{
    INTERFACE_AS_SERVICE(SmsService, RIL_SERVICE_SMS)
    friend SmsServiceProxy;
public:
    char m_sca[MAX_GSM_SMS_SERVICE_CENTER_ADDR];
    int  m_scaLen;
    int  m_smsClass;

    SmsService(RilContext* pRilContext);
    virtual ~SmsService();
protected:
    virtual int OnCreate(RilContext *pRilContext);

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);

    void OnSimStatusChanged(int cardState, int appState);

    virtual bool IsPossibleToPassInRadioOffState(int request_id);
protected:
    virtual int DoSendCdmaSms(Message *pMsg, bool bExpectMore);
    virtual int OnSendCdmaSmsDone(Message *pMsg);
    virtual int DoWriteCdmaSmsToRuim(Message *pMsg);
    virtual int OnWriteCdmaSmsToRuimDone(Message *pMsg);
    virtual int DoDeleteCdmaSmsOnRuim(Message *pMsg);
    virtual int OnDeleteCdmaSmsOnRuimDone(Message *pMsg);
    virtual int DoGetCdmaBroadcastSmsConfig(Message *pMsg);
    virtual int OnGetCdmaBroadcastSmsConfigDone(Message *pMsg);
    virtual int DoSetCdmaBroadcastSmsConfig(Message *pMsg);
    virtual int OnSetCdmaBroadcastSmsConfigDone(Message *pMsg);
    virtual int DoSmsCdmaBroadcastActivation(Message *pMsg);
    virtual int OnSmsCdmaBroadcastActivationDone(Message *pMsg);

    virtual int OnIncomingNewCdmaSms(Message *pMsg);
    virtual int OnRuimSmsStorageFull(Message *pMsg);
    virtual int OnVoiceMsgWaitingInfo(Message *pMsg);

    virtual int DoSendAimsSms(Message *pMsg);
    virtual int OnSendAimsSmsDone(Message *pMsg);
};
#endif /* __SMS_SERVICE_H__ */
