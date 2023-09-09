/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "apimshandlerfactory.h"
#include "messageid.h"
#include "apimshandler.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(ApImsHandlerFactory)

BEGIN_FACTORY_ENTRY(ApImsHandlerFactory)
    ADD_FACTORY_ENTRY(UpdateImsRegistrationInfoHandler, MSG_IMS_UPDATE_IMS_REGISTRATION_INFO)
    ADD_FACTORY_ENTRY(TriggerImsDeregistrationHandler, MSG_IMS_UNSOL_TRIGGER_IMS_DEREGISTRATION)
    ADD_FACTORY_ENTRY(StartImsTrafficHandler, MSG_IMS_START_IMS_TRAFFIC)
    ADD_FACTORY_ENTRY(StopImsTrafficHandler, MSG_IMS_STOP_IMS_TRAFFIC)
    ADD_FACTORY_ENTRY(OnConnectionSetupFailureHandler, MSG_IMS_UNSOL_CONNECTION_SETUP_FAILURE)
    ADD_FACTORY_ENTRY(TriggerEpsFallbackHandler, MSG_IMS_TRIGGER_EPS_FALLBACK)
    ADD_FACTORY_ENTRY(IndTriggerEpsFallbackHandler, MSG_IMS_IND_TRIGGER_EPS_FALLBACK)
    ADD_FACTORY_ENTRY(SetSrvccCallInfoHandler, MSG_IMS_SET_SRVCC_CALL_INFO)
    ADD_FACTORY_ENTRY(MtCallGuardtmrStatusHandler, MSG_IMS_MT_CALL_GUARDTMR_STATUS_IND)
    ADD_FACTORY_ENTRY(MtCsfbHandler, MSG_IMS_IND_MT_CSFB)
END_FACTORY_ENTRY()
