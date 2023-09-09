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
 * callhandler.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __CALL_HANDLER_H__
#define __CALL_HANDLER_H__

#include "callreqdata.h"
#include "servicehandlerbase.h"

/**
 * GetCurrentCallListHandler
 */
class GetCurrentCallListHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(GetCurrentCallListHandler)
protected:
    void OnCreate();
private:
    static const int MAX_CALL_LIST_COUNT = CallList::MAX_CALL_LIST_COUNT;
    RIL_Call *mRespCalls[MAX_CALL_LIST_COUNT] = {};
    RIL_Call mRespCallsData[MAX_CALL_LIST_COUNT] = {};
    RIL_Call_V1_2 *mRespCalls_V1_2[MAX_CALL_LIST_COUNT] = {};
    RIL_Call_V1_2 mRespCallsData_V1_2[MAX_CALL_LIST_COUNT] = {};
    RIL_Call_V1_6 *mRespCalls_V1_6[MAX_CALL_LIST_COUNT] = {};
    RIL_Call_V1_6 mRespCallsData_V1_6[MAX_CALL_LIST_COUNT] = {};
    char *BuildCallListResponse(CallList *data, int *len);
    int GetAudioQuality();
};

/**
 * DialHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DialHandler)

/**
 * EmergencyDialHandler
 */
class EmergencyDialHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(EmergencyDialHandler);
private:
    int isNwPlmnAllowedForNormalRoutingEccUnderFDN();
};

/**
 * AnswerHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(AnswerHandler)

/**
 * ExplicitCallTransferHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ExplicitCallTransferHandler)

/**
 * HangupHandler
 */
class HangupHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(HangupHandler)
protected:
    int DoReleaseCall(int callId);
    int DoReleaseCallMulti(int callId);
};

/**
 * GetLastCallFailCauseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetLastCallFailCauseHandler)

/**
 * RingbackToneHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(RingbackToneHandler)

/**
 * CallStateChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CallStateChangedHandler)

/**
 * CallRingingHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CallRingingHandler)

/**
 * EmergencySupportRatModeHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(EmergencySupportRatModeHandler)

/**
 * EmergencyCallListReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(EmergencyCallListReceivedHandler)

/**
 * UdubHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(UdubHandler)

/**
 * HangupFgResumeBgHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(HangupFgResumeBgHandler)

/**
 * HangupWaitOrBgHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(HangupWaitOrBgHandler)

/**
 * SwitchWaitOrHoldAndActiveHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SwitchWaitOrHoldAndActiveHandler)

/**
 * ConferenceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ConferenceHandler)

/**
 * SeparateConnectionHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SeparateConnectionHandler)

/**
 * SetCallConfirmHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCallConfirmHandler)

/**
 * SendCallConfirmHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendCallConfirmHandler)

/**
 * UnsolCallPresentHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCallPresentHandler)

/**
 * UnsolEnterEmergencyCallbackModeHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolEnterEmergencyCallbackModeHandler)

/**
 * UnsolExitEmergencyCallbackModeHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolExitEmergencyCallbackModeHandler)

/**
 * ExitEmergencyCallbackModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ExitEmergencyCallbackModeHandler)

#endif /* __CALL_HANDLER_H__ */
