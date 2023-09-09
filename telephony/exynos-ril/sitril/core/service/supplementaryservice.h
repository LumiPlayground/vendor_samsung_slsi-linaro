/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SUPPLEMENTARYSERVICE_H__
#define __SUPPLEMENTARYSERVICE_H__

#include "service.h"
#include "calldata.h"

#define SUPPLEMENTARY_DEFAULT_TIMEOUT        60000

class SuppServiceProxy;

class SupplementaryService :public Service
{
    INTERFACE_AS_SERVICE(SupplementaryService, RIL_SERVICE_SUPPLEMENTARY)
    friend SuppServiceProxy;
public:
    SupplementaryService(RilContext* pRilContext);
    virtual ~SupplementaryService();

protected:
    virtual int OnCreate(RilContext *pRilContext);

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleInternalMessage(Message* pMsg);
    virtual void OnSimStatusChanged(int cardState, int appState);

protected:
    virtual INT32 DoSendEncodedUssd();

    BOOL IsNullRequest(Message *pMsg);
    virtual bool IsPossibleToPassInRadioOffState(int request_id);
    int GetValidErrors(int errorCode);
    bool IsOperatorUsingUnknownServiceClass();

protected:
    int mCardState;
    ClirInfo m_clirInfo;

private:
    BOOL m_ussdUserInitiated;
};
#endif
