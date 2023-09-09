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
 * supphandler.h
 */

#ifndef __SUPP_HANDLER_H__
#define __SUPP_HANDLER_H__

#include <telephony/ril.h>
#include "servicehandlerbase.h"

/**
 * GetCallWaitingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCallWaitingHandler)

/**
 * SetCallWaitingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCallWaitingHandler)

/**
 * GetCallForwardingHandler
 */
class GetCallForwardingHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(GetCallForwardingHandler)
protected:
    void OnCreate();
private:
    static const int MAX_CALL_FORWARD_LIST_COUNT = 8;

    RIL_CallForwardInfo *mRespCallForward[MAX_CALL_FORWARD_LIST_COUNT] = {};
    RIL_CallForwardInfo mRespCallForwardData[MAX_CALL_FORWARD_LIST_COUNT] = {};
};

/**
 * SetCallForwardingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCallForwardingHandler)

/**
 * ChangeCallBarringPwdHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ChangeCallBarringPwdHandler)

/**
 * GetClirHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetClirHandler)

/**
 * SetClirHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetClirHandler)

/**
 * GetClipHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetClipHandler)

/**
 * GetColrHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetColrHandler)

/**
 * GetColpHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetColpHandler)

/**
 * SendUssdHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendUssdHandler)

/**
 * CancelUssdHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CancelUssdHandler)

/**
 * OnUssdHandler
 */
class OnUssdHandler : public ServiceHandlerBase {
    DECLARE_TINY_UNSOL_SERVICE_HANDLER_INIT(OnUssdHandler)
private:
    static const int MAX_USSD_COUNT = 3;
};

/**
 * OnSsSvcHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(OnSsSvcHandler)

/**
 * OnUnsolOnSsHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(OnUnsolOnSsHandler)
#endif /* __SUPP_HANDLER_H__ */
