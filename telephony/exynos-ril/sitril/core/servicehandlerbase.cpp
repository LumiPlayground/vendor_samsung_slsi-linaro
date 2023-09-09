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
 * servicehandlerbase.cpp
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#include "rilcontext.h"
#include "rillog.h"
#include "service.h"
#include "servicehandlerbase.h"

// constructor/destructor
ServiceHandlerBase::ServiceHandlerBase(int messageId, Service *service)
    : mMessageId(messageId), mService(service), mServiceName("")
    , mRilContext(NULL), mPhoneId(-1)
{
    if (mService != NULL) {
        mServiceName = mService->GetServiceName();
        mRilContext = mService->GetRilContext();
        mPhoneId = mService->GetRilSocketId();
    }
}

ServiceHandlerBase::~ServiceHandlerBase()
{
    mService = NULL;
    mRilContext = NULL;
}

bool ServiceHandlerBase::OnHandleRequest(Message *msg)
{
    if (msg == NULL || mService == NULL || msg->GetMsgId() != mMessageId) {
        return false;
    }
    return (OnRequest(msg) == 0);
}

bool ServiceHandlerBase::OnHandleSolicitedResponse(Message *msg)
{
    if (msg == NULL || mService == NULL || msg->GetMsgId() != mMessageId) {
        return false;
    }
    return (OnResponse(msg) == 0);
}

bool ServiceHandlerBase::OnHandleUnsolicitedResponse(Message *msg)
{
    if (msg == NULL || mService == NULL || msg->GetMsgId() != mMessageId) {
        return false;
    }

    return (OnIndication(msg) == 0);
}

bool ServiceHandlerBase::OnHandleRequestTimeout(Message *msg)
{
    if (msg == NULL || mService == NULL || msg->GetMsgId() != mMessageId) {
        return false;
    }
    return (OnTimeout(msg) == 0);
}

bool ServiceHandlerBase::OnHandleInternalMessage(Message * /* msg */)
{
    return false;
}

int ServiceHandlerBase::OnRequest(Message * /* msg */)
{
    return -1;
}

int ServiceHandlerBase::OnResponse(Message * /* msg */)
{
    return -1;
}

int ServiceHandlerBase::OnIndication(Message * /* msg */)
{
    return -1;
}

int ServiceHandlerBase::OnTimeout(Message * /* msg */)
{
    return -1;
}

int ServiceHandlerBase::SendRequest(ModemData *modemData, uint32_t timeout, uint32_t result)
{
    if (mService == NULL) {
        return -1;
    }
    return mService->SendRequest(modemData, timeout, result);
}

int ServiceHandlerBase::SendRequest(ModemData *modemData)
{
    if (mService == NULL) {
        return -1;
    }
    return mService->SendRequest(modemData);
}

int ServiceHandlerBase::OnRequestComplete(int result, void *data/* = NULL*/, size_t length/* = 0*/)
{
    if (mService == NULL) {
        return -1;
    }
    return mService->OnRequestComplete(result, data, length);
}

int ServiceHandlerBase::OnUnsolicitedResponse(int id, const void *data/* = NULL*/, size_t length/* = 0*/)
{
    if (mService == NULL) {
        return -1;
    }
    return mService->OnUnsolicitedResponse(id, data, length);
}
