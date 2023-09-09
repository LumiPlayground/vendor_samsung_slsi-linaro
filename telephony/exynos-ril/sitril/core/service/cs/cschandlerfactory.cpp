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
 * cschandlerfactory.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include "callhandler.h"
#include "cschandlerfactory.h"
#include "messageid.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(CscHandlerFactory)

BEGIN_FACTORY_ENTRY(CscHandlerFactory)
    ADD_FACTORY_ENTRY(GetCurrentCallListHandler, MSG_CS_CALL_LIST)
    ADD_FACTORY_ENTRY(DialHandler, MSG_CS_CALL_DIAL)
    ADD_FACTORY_ENTRY(EmergencyDialHandler, MSG_CS_CALL_EMERGENCY_DIAL)
    ADD_FACTORY_ENTRY(AnswerHandler, MSG_CS_CALL_ANSWER)
    ADD_FACTORY_ENTRY(ExplicitCallTransferHandler, MSG_CS_SS_EXPLICIT_CALL_TRANSFER)
    ADD_FACTORY_ENTRY(HangupHandler, MSG_CS_CALL_HANGUP)
    ADD_FACTORY_ENTRY(GetLastCallFailCauseHandler, MSG_CS_LAST_CALL_FAIL_CAUSE)
    ADD_FACTORY_ENTRY(RingbackToneHandler, MSG_CS_SOUND_RINGBACKTONE_NTF)
    ADD_FACTORY_ENTRY(CallStateChangedHandler, MSG_CS_CALL_STATE_CHANGE_NTF)
    ADD_FACTORY_ENTRY(CallRingingHandler, MSG_CS_CALL_RINGING_NTF)
    ADD_FACTORY_ENTRY(EmergencySupportRatModeHandler, MSG_CS_IND_EMERGENCY_SUPPORT_RAT_MODE)
    ADD_FACTORY_ENTRY(EmergencyCallListReceivedHandler, MSG_NET_EMERGENCY_CALL_LIST_RECEIVED)
    ADD_FACTORY_ENTRY(UdubHandler, MSG_CS_SS_UDUB)
    ADD_FACTORY_ENTRY(HangupFgResumeBgHandler, MSG_CS_SS_HANGUP_FOREGROUND_RESUME_BACKGROUND)
    ADD_FACTORY_ENTRY(HangupWaitOrBgHandler, MSG_CS_SS_HANGUP_WAITING_OR_BACKGROUND)
    ADD_FACTORY_ENTRY(SwitchWaitOrHoldAndActiveHandler, MSG_CS_SS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE)
    ADD_FACTORY_ENTRY(ConferenceHandler, MSG_CS_SS_CONFERENCE)
    ADD_FACTORY_ENTRY(SeparateConnectionHandler, MSG_CS_SS_SEPARATE_CONNECTION)
    ADD_FACTORY_ENTRY(SetCallConfirmHandler, MSG_CS_SEND_CALL_CONFIRM)
    ADD_FACTORY_ENTRY(UnsolCallPresentHandler, MSG_CS_CALL_PRESENT_NTF)
    ADD_FACTORY_ENTRY(UnsolEnterEmergencyCallbackModeHandler, MSG_CS_IND_ENTER_EMERGENCY_CB_MODE_NTF)
    ADD_FACTORY_ENTRY(UnsolExitEmergencyCallbackModeHandler, MSG_CS_IND_EXIT_EMERGENCY_CB_MODE_NTF)
    ADD_FACTORY_ENTRY(ExitEmergencyCallbackModeHandler, MSG_CS_EXIT_EMERGENCY_CB_MODE)
END_FACTORY_ENTRY()
