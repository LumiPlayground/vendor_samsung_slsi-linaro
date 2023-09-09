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
 * imshandler.h
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#ifndef __IMS_HANDLER_H__
#define __IMS_HANDLER_H__

#include "servicehandlerbase.h"

class AimsDefaultHandler : public ServiceHandlerBase
{
    DECLARE_MODULE_TAG()
public:
    AimsDefaultHandler(Service *service)
        : ServiceHandlerBase(-1, service) {}
    virtual ~AimsDefaultHandler() {}
    virtual bool OnHandleRequest(Message *msg);
    virtual bool OnHandleSolicitedResponse(Message *msg);
    virtual bool OnHandleUnsolicitedResponse(Message *msg);
    const char *GetName() { return TAG; }

    static bool CanHandleRequest(int messageId);
    static bool CanHandleRequestInd(int messageId);
    static bool CanHandleIndication(int messageId);
protected:
    virtual int OnRequest(Message *msg);
    virtual int OnRequestInd(Message *msg);
    virtual int OnResponse(Message *msg);
    virtual int OnIndication(Message *msg);

public:
    static AimsDefaultHandler *Create(Service *service);
};

#endif /* __IMS_HANDLER_H__ */
