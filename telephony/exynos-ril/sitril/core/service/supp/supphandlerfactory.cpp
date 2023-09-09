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
 * supphandlerfactory.cpp
 */

#include "messageid.h"
#include "supphandler.h"
#include "supphandlerfactory.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(SuppHandlerFactory)

BEGIN_FACTORY_ENTRY(SuppHandlerFactory)
    // Request and SolicitedResponse Handler
    ADD_FACTORY_ENTRY(GetCallWaitingHandler, MSG_SS_QUERY_CALL_WAITING)
    ADD_FACTORY_ENTRY(SetCallWaitingHandler, MSG_SS_SET_CALL_WAITING)
    ADD_FACTORY_ENTRY(GetCallForwardingHandler, MSG_SS_QUERY_CALL_FORWARDING)
    ADD_FACTORY_ENTRY(SetCallForwardingHandler, MSG_SS_SET_CALL_FORWARDING)
    ADD_FACTORY_ENTRY(ChangeCallBarringPwdHandler, MSG_SS_CALL_BARRING_PWD)
    ADD_FACTORY_ENTRY(GetClirHandler, MSG_SS_GET_CLIR)
    ADD_FACTORY_ENTRY(SetClirHandler, MSG_SS_SET_CLIR)
    ADD_FACTORY_ENTRY(GetClipHandler, MSG_SS_GET_CLIP)
    ADD_FACTORY_ENTRY(GetColrHandler, MSG_SS_QUERY_COLR)
    ADD_FACTORY_ENTRY(GetColpHandler, MSG_SS_QUERY_COLP)
    ADD_FACTORY_ENTRY(SendUssdHandler, MSG_SS_SEND_USSD)
    ADD_FACTORY_ENTRY(CancelUssdHandler, MSG_SS_CANCEL_USSD)

    // UnsolicitedResponse Handler
    ADD_FACTORY_ENTRY(OnUssdHandler, MSG_SS_USSD_NTF)
    ADD_FACTORY_ENTRY(OnSsSvcHandler, MSG_SS_SVC_NTF)
    ADD_FACTORY_ENTRY(OnUnsolOnSsHandler, MSG_SS_IND)
END_FACTORY_ENTRY()
