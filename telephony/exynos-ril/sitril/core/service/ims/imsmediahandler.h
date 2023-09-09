/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */


#ifndef __IMSMEDIA_HANDLER_H__
#define __IMSMEDIA_HANDLER_H__

#include "servicehandlerbase.h"

class ImsMediaDefaultHandler : public ServiceHandlerBase
{
    DECLARE_MODULE_TAG()
public:
    ImsMediaDefaultHandler(Service *service)
            : ServiceHandlerBase(-1, service) {}
    virtual ~ImsMediaDefaultHandler() {}
    virtual bool OnHandleRequest(Message *msg);
    virtual bool OnHandleSolicitedResponse(Message *msg);
    virtual bool OnHandleUnsolicitedResponse(Message *msg);
    const char *GetName() { return TAG; }

    static bool CanHandleRequest(int messageId);
    static bool CanHandleRequestInd(int messageId);
    static bool CanHandleIndication(int messageId);

    static ImsMediaDefaultHandler *Create(Service *service);
protected:
    virtual int OnRequest(Message *msg);
    virtual int OnRequestInd(Message *msg);
    virtual int OnResponse(Message *msg);
    virtual int OnIndication(Message *msg);
};

#endif /* __IMSMEDIA_HANDLER_H__ */
